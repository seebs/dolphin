// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <algorithm>
#include <vector>

#include <QIcon>

#include "Core/Core.h"
#include "DolphinQt2/QtUtils/ActionHelper.h"
#include "DolphinQt2/Resources.h"
#include "DolphinQt2/Settings.h"
#include "DolphinQt2/ToolBar.h"

static QSize ICON_SIZE(32, 32);

ToolBar::ToolBar(QWidget* parent) : QToolBar(parent)
{
  setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  setMovable(false);
  setFloatable(false);
  setIconSize(ICON_SIZE);

  MakeActions();
  connect(&Settings::Instance(), &Settings::ThemeChanged, this, &ToolBar::UpdateIcons);
  UpdateIcons();

  connect(&Settings::Instance(), &Settings::EmulationStateChanged, this,
          [this](Core::State state) { OnEmulationStateChanged(state); });

  connect(&Settings::Instance(), &Settings::DebugModeToggled, this, &ToolBar::OnDebugModeToggled);

  OnEmulationStateChanged(Core::GetState());
  OnDebugModeToggled(Settings::Instance().IsDebugModeEnabled());
}

void ToolBar::OnEmulationStateChanged(Core::State state)
{
  bool running = state != Core::State::Uninitialized;
  m_stop_action->setEnabled(running);
  m_fullscreen_action->setEnabled(running);
  m_screenshot_action->setEnabled(running);

  bool playing = running && state != Core::State::Paused;
  m_play_action->setEnabled(!playing);
  m_play_action->setVisible(!playing);
  m_pause_action->setEnabled(playing);
  m_pause_action->setVisible(playing);
}

void ToolBar::OnDebugModeToggled(bool enabled)
{
  m_step_action->setVisible(enabled);
  m_step_over_action->setVisible(enabled);
  m_step_out_action->setVisible(enabled);
  m_skip_action->setVisible(enabled);
  m_show_pc_action->setVisible(enabled);
  m_set_pc_action->setVisible(enabled);
}

void ToolBar::MakeActions()
{
  m_step_action = AddAction(this, tr("Step"), this, &ToolBar::StepPressed);
  m_step_over_action = AddAction(this, tr("Step Over"), this, &ToolBar::StepOverPressed);
  m_step_out_action = AddAction(this, tr("Step Out"), this, &ToolBar::StepOutPressed);
  m_skip_action = AddAction(this, tr("Skip"), this, &ToolBar::SkipPressed);
  m_show_pc_action = AddAction(this, tr("Show PC"), this, &ToolBar::ShowPCPressed);
  m_set_pc_action = AddAction(this, tr("Set PC"), this, &ToolBar::SetPCPressed);

  m_open_action = AddAction(this, tr("Open"), this, &ToolBar::OpenPressed);
  m_play_action = AddAction(this, tr("Play"), this, &ToolBar::PlayPressed);
  m_pause_action = AddAction(this, tr("Pause"), this, &ToolBar::PausePressed);
  m_stop_action = AddAction(this, tr("Stop"), this, &ToolBar::StopPressed);
  m_fullscreen_action = AddAction(this, tr("FullScr"), this, &ToolBar::FullScreenPressed);
  m_screenshot_action = AddAction(this, tr("ScrShot"), this, &ToolBar::ScreenShotPressed);

  addSeparator();

  m_config_action = AddAction(this, tr("Config"), this, &ToolBar::SettingsPressed);
  m_graphics_action = AddAction(this, tr("Graphics"), this, &ToolBar::GraphicsPressed);
  m_controllers_action = AddAction(this, tr("Controllers"), this, &ToolBar::ControllersPressed);
  m_controllers_action->setEnabled(true);

  // Ensure every button has about the same width
  std::vector<QWidget*> items;
  for (const auto& action :
       {m_open_action, m_play_action, m_pause_action, m_stop_action, m_stop_action,
        m_fullscreen_action, m_screenshot_action, m_config_action, m_graphics_action,
        m_controllers_action, m_step_action, m_step_over_action, m_step_out_action, m_skip_action,
        m_show_pc_action, m_set_pc_action})
  {
    items.emplace_back(widgetForAction(action));
  }

  std::vector<int> widths;
  std::transform(items.begin(), items.end(), std::back_inserter(widths),
                 [](QWidget* item) { return item->sizeHint().width(); });

  const int min_width = *std::max_element(widths.begin(), widths.end()) * 0.85;
  for (QWidget* widget : items)
    widget->setMinimumWidth(min_width);
}

void ToolBar::UpdateIcons()
{
  m_step_action->setIcon(Resources::GetScaledThemeIcon("debugger_step_in"));
  m_step_over_action->setIcon(Resources::GetScaledThemeIcon("debugger_step_over"));
  m_step_out_action->setIcon(Resources::GetScaledThemeIcon("debugger_step_out"));
  m_skip_action->setIcon(Resources::GetScaledThemeIcon("debugger_skip"));
  m_show_pc_action->setIcon(Resources::GetScaledThemeIcon("debugger_set_pc"));
  m_set_pc_action->setIcon(Resources::GetScaledThemeIcon("debugger_show_pc"));

  m_open_action->setIcon(Resources::GetScaledThemeIcon("open"));
  m_play_action->setIcon(Resources::GetScaledThemeIcon("play"));
  m_pause_action->setIcon(Resources::GetScaledThemeIcon("pause"));
  m_stop_action->setIcon(Resources::GetScaledThemeIcon("stop"));
  m_fullscreen_action->setIcon(Resources::GetScaledThemeIcon("fullscreen"));
  m_screenshot_action->setIcon(Resources::GetScaledThemeIcon("screenshot"));
  m_config_action->setIcon(Resources::GetScaledThemeIcon("config"));
  m_controllers_action->setIcon(Resources::GetScaledThemeIcon("classic"));
  m_graphics_action->setIcon(Resources::GetScaledThemeIcon("graphics"));
}
