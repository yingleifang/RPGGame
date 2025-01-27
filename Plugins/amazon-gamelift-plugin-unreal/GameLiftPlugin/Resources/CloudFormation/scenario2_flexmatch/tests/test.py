# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import string
import boto3
import requests
import json
import time
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-g", "--game", help="game name", type=str, required=True)
parser.add_argument("-r", "--region", help="region name, e.g. eu-west-1", type=str, required=True)
parser.add_argument("-p", "--profile", help="profile name in the AWS shared credentials file ~/.aws/credentials", type=str, required=True)
args = parser.parse_args()

GAME_NAME = args.game.lower() # e.g. 'GameLiftSampleGame5ue4'
REGION = args.region # e.g. 'eu-west-1'
PROFILE_NAME = args.profile

USER_POOL_NAME = GAME_NAME + 'UserPool'
USER_POOL_CLIENT_NAME = GAME_NAME + 'UserPoolClient'
USERNAME1 = 'testuser1@example.com'
USERNAME2 = 'testuser2@example.com'
USERNAMES = [USERNAME1, USERNAME2]
PASSWORD = 'TestPassw0rd.'
REST_API_NAME = GAME_NAME + 'RestApi'
REST_API_STAGE = 'dev'
GAME_REQUEST_PATH = 'start_game'
RESULTS_REQUEST_PATH = 'get_game_connection'
session = boto3.Session(profile_name=PROFILE_NAME)
cognito_idp = session.client('cognito-idp', region_name=REGION)
apig = session.client('apigateway', region_name=REGION)
REGION_US_WEST_2 = 'us-west-2'
REGION_EU_WEST_1 = 'eu-west-1'
REGION_US_EAST_1 = 'us-east-1'
NO_LATENCY = 'no-latency'
REGIONS_TO_TEST = [REGION_US_WEST_2, REGION_EU_WEST_1, NO_LATENCY]
US_WEST_2_FIRST_REGION_TO_LATENCY_MAPPING = {
    "regionToLatencyMapping": {
        "us-west-2": 50,
        "us-east-1": 100,
        "eu-west-1": 150,
        "ap-northeast-1": 300
    }
}
EU_WEST_1_FIRST_REGION_TO_LATENCY_MAPPING = {
    "regionToLatencyMapping": {
        "us-west-2": 50,
        "us-east-1": 100,
        "eu-west-1": 10,
        "ap-northeast-1": 300
    }
}
REGION_TO_GAME_REQUEST_PAYLOAD_MAPPING = {
    REGION_US_WEST_2: json.dumps(US_WEST_2_FIRST_REGION_TO_LATENCY_MAPPING),
    REGION_EU_WEST_1: json.dumps(EU_WEST_1_FIRST_REGION_TO_LATENCY_MAPPING),
    NO_LATENCY: None
}
REGION_TO_GAME_SESSION_ARN_EXPECTED_LOCATION = {
    REGION_US_WEST_2: REGION_US_WEST_2,
    REGION_EU_WEST_1: REGION_EU_WEST_1,
    NO_LATENCY: REGION_US_EAST_1
}


def main():

    user_pool = find_user_pool(USER_POOL_NAME)
    user_pool_id = user_pool['Id']
    print("User Pool Id:", user_pool_id)

    user_pool_client = find_user_pool_client(user_pool_id, USER_POOL_CLIENT_NAME)
    user_pool_client_id = user_pool_client['ClientId']
    print("User Pool Client Id:", user_pool_client_id)

    try:
        for region in REGIONS_TO_TEST:
            game_request_payload = REGION_TO_GAME_REQUEST_PAYLOAD_MAPPING.get(region)
            expected_game_session_region = REGION_TO_GAME_SESSION_ARN_EXPECTED_LOCATION.get(region)

            headers_list = []
            for username in USERNAMES:
                regional_username = get_regional_user_name(username, region)

                cognito_idp.sign_up(
                    ClientId=user_pool_client_id,
                    Username=regional_username,
                    Password=PASSWORD,
                )

                print(f"Created user: {regional_username}")

                cognito_idp.admin_confirm_sign_up(
                    UserPoolId=user_pool_id,
                    Username=regional_username,
                )

                init_auth_result = cognito_idp.initiate_auth(
                    AuthFlow='USER_PASSWORD_AUTH',
                    AuthParameters={
                        'USERNAME': regional_username,
                        'PASSWORD': PASSWORD,
                    },
                    ClientId=user_pool_client_id
                )

                assert init_auth_result['ResponseMetadata']['HTTPStatusCode'] == 200, "Unsuccessful init_auth"
                print(f"Authenticated via username and password for {regional_username}")

                id_token = init_auth_result['AuthenticationResult']['IdToken']
                headers = {
                    'Auth': id_token
                }
                headers_list.append(headers)

            results_request_url = get_rest_api_endpoint(REST_API_NAME, REGION, REST_API_STAGE, RESULTS_REQUEST_PATH)
            game_request_url = get_rest_api_endpoint(REST_API_NAME, REGION, REST_API_STAGE, GAME_REQUEST_PATH)

            print(f"results_request_url: {results_request_url}")
            print(f"game_request_url: {game_request_url}")

            for headers in headers_list:
                results_request_response = requests.post(url=results_request_url, headers=headers)
                assert results_request_response.status_code == 404, \
                    f"Expect 'POST /get_game_connection' status code to be 404 (Not Found). Actual: " \
                    f"{str(results_request_response.status_code)}"
                print("Verified lambda ResultsRequest response", results_request_response)

                game_request_response = requests.post(url=game_request_url, headers=headers, data=game_request_payload)

                print(f"Game request response '{game_request_response}'")

                assert game_request_response.status_code == 202, \
                    f"Expect 'POST /start_game' status code to be 202 (Accepted), actual: " \
                    f"{str(game_request_response.status_code)}"
                print("Verified lambda GameRequest response", game_request_response)

            print("Waiting for matchmaking request to be processed...")

            verified_players = 0
            while verified_players != len(headers_list):
                verified_players = 0
                time.sleep(10)  # 10 seconds
                for headers in headers_list:
                    results_request_response = requests.post(url=results_request_url, headers=headers)
                    if results_request_response.status_code == 204:
                        print("Match is not ready yet")
                        continue
                    assert results_request_response.status_code == 200, \
                        f"Expect 'POST /get_game_connection' status code to be 200 (Success), actual: " \
                        f"{str(results_request_response.status_code)}"
                    print("Verified lambda ResultsRequest response", results_request_response)

                    game_connection_info = json.loads(results_request_response.content)

                    print(f"Game connection info '{game_connection_info}'")

                    assert game_connection_info['IpAddress'] != ''
                    assert int(game_connection_info['Port']) > 0
                    assert REGION in game_connection_info['DnsName'], \
                        f"Expect {game_connection_info['DnsName']} to contain '{REGION}'"
                    assert expected_game_session_region in game_connection_info['GameSessionArn'], \
                        f"Expect {game_connection_info['GameSessionArn']} to contain '{expected_game_session_region}'"
                    assert "psess-" in game_connection_info['PlayerSessionId'], \
                        f"Expect {game_connection_info['PlayerSessionId']} to contain 'psess-'"
                    print("Verified game connection info:", game_connection_info)
                    verified_players += 1
                print(f"{verified_players} players' game sessions verified")

    finally:
        for region in REGIONS_TO_TEST:
            for username in USERNAMES:
                regional_username = get_regional_user_name(username, region)
                cognito_idp.admin_delete_user(
                    UserPoolId=user_pool_id,
                    Username=regional_username,
                )
                print("Deleted user:", regional_username)

        print("Test Succeeded!")


def get_regional_user_name(username, region):
    return f"{region}_{username}"


def find_user_pool(user_pool_name):
    print("Finding user pool:", user_pool_name)
    result = cognito_idp.list_user_pools(MaxResults=50)
    pools = result['UserPools']
    return next(x for x in pools if x['Name'] == user_pool_name)


def find_user_pool_client(user_pool_id, user_pool_client_name):
    print("Finding user pool client:", user_pool_client_name)
    results = cognito_idp.list_user_pool_clients(UserPoolId=user_pool_id)
    clients = results['UserPoolClients']
    return next(x for x in clients if x['ClientName'] == user_pool_client_name)


def find_rest_api(rest_api_name):
    print("Finding rest api:", rest_api_name)
    results = apig.get_rest_apis()
    rest_apis = results['items']
    return next(x for x in rest_apis if x['name'] == rest_api_name)


def get_rest_api_endpoint(rest_api_name, region, stage, path):
    print("Getting rest api endpoint", rest_api_name)
    rest_api = find_rest_api(rest_api_name)
    rest_api_id = rest_api['id']
    return f'https://{rest_api_id}.execute-api.{region}.amazonaws.com/{stage}/{path}'


if __name__ == '__main__':
    main()
