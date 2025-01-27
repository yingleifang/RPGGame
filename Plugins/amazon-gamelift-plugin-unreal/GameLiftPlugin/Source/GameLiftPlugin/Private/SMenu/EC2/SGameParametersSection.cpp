// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SGameParametersSection.h"

#include "Settings/UGameLiftDeploymentStatus.h"
#include "Settings/UGameLiftSettings.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/STextSeparator.h"
#include "SWidgets/SDeploymentFields.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "Types/EManagedEC2DeploymentScenario.h"
#include "Types/EDeploymentMessageState.h"

#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "IGameLiftCoreModule.h"

#include "SGameLiftDeployManagedEC2Menu.h"
#include "SDeployScenarioSection.h"

#define LOCTEXT_NAMESPACE "SGameParametersSection"

void SGameParametersSection::Construct(const FArguments& InArgs)
{
	DeploymentFields = InArgs._SetDeploymentFields;

	SSectionStep::Construct(
		SSectionStep::FArguments()
		.HeaderTitle(Menu::DeployManagedEC2::kGameParametersHeader)
		.HeaderDescription(Menu::DeployManagedEC2::kGameParametersDescription)
		.BodyContent()
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					DeploymentFields.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				.Padding(SPadding::Top3x)
				[
					SAssignNew(SectionSwitcher, SWidgetSwitcher)
						+ SWidgetSwitcher::Slot()
						[
							CreateSubmissionButton()
						]
						+ SWidgetSwitcher::Slot()
						[
							CreateModifyButton()
						]
				]
		]);

	SDeployScenarioSection::OnEC2DeploymentProgressChangedMultiDelegate.AddSP(this, &SGameParametersSection::UpdateUIBasedOnCurrentState);
	UpdateUIBasedOnCurrentState();
}

TSharedRef<SWidget> SGameParametersSection::CreateSubmissionButton()
{
	return SAssignNew(SubmissionButton, SButton)
		.Text(Menu::DeployManagedEC2::kSetParametersForDeploymentButtonText)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
		.OnClicked_Raw(this, &SGameParametersSection::OnSubmissionButtonClicked)
		.IsEnabled_Lambda([&]()
			{
				return AreDeploySettingsComplete();
			});
}

TSharedRef<SWidget> SGameParametersSection::CreateModifyButton()
{
	return SAssignNew(ModifyButton, SButton)
		.Text(Menu::DeployManagedEC2::kModifyParametersButtonText)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
		.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
		.OnClicked_Raw(this, &SGameParametersSection::OnModifyButtonClicked);
}

FReply SGameParametersSection::OnSubmissionButtonClicked()
{
	CompleteSection();
	StartNextSection();

	return FReply::Handled();
}

bool SGameParametersSection::AreDeploySettingsComplete()
{
	// ExtraServerResourcesPath field isn't visible so we exclude it here
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	return !DeploymentInfo->GetBuildName().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetBuildOperatingSystem().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetBuildFolderPath().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetBuildFilePath().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetOutConfigFilePath().IsEmptyOrWhitespace();
}

FReply SGameParametersSection::OnModifyButtonClicked()
{
	ResetAndCollapseNextSections();

	return FReply::Handled();
}

void SGameParametersSection::UpdateUIBasedOnCurrentState()
{
	// TODO: Add state for each section completion
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	EDeploymentMessageState State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());

	ModifyButton->SetEnabled(State != EDeploymentMessageState::InProgressMessage);

	if (State != EDeploymentMessageState::NoDeploymentMessage)
	{
		CompleteSection();
	}
	else
	{
		ResetAndCollapseSection();
	}
}

void SGameParametersSection::CompleteSection()
{
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	DeploymentInfo->SetAllFieldsReadOnly(true);
	SectionSwitcher->SetActiveWidgetIndex((int32)ESectionUIState::Complete);
	SetProgressBarState(SProgressBar::EProgressBarUIState::ProgressComplete);
}

void SGameParametersSection::StartSection()
{
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	DeploymentInfo->SetAllFieldsReadOnly(false);
	SectionSwitcher->SetActiveWidgetIndex((int32)ESectionUIState::InComplete);
}

#undef LOCTEXT_NAMESPACE