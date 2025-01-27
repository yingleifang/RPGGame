// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

enum class ESetupMessageState
{
	HelpMessage,
	InfoMessage,
	WarningMessage,
	FailureMessage,
	ReadyToGoMessage,
	DismissedMessage
};
