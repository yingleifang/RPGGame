// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "SWidgets/SDeploymentFields.h"
#include "SWidgets/SSectionStep.h"

class SGameParametersSection : public SSectionStep
{
	SLATE_BEGIN_ARGS(SGameParametersSection) { }

	SLATE_ARGUMENT(TSharedPtr<SWidget>, SetDeploymentFields)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> CreateSubmissionButton();
	TSharedRef<SWidget> CreateModifyButton();
	FReply OnSubmissionButtonClicked();
	FReply OnModifyButtonClicked();
	
	void UpdateUIBasedOnCurrentState();
	bool AreDeploySettingsComplete();
	void CompleteSection();
	void StartSection() override;

private:
	TSharedPtr<SWidget> DeploymentFields;
	TSharedPtr<SWidgetSwitcher> SectionSwitcher;
	TSharedPtr<SButton> SubmissionButton;
	TSharedPtr<SButton> ModifyButton;
};
