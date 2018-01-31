// Copyright 2018 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DolphinQt2/TAS/WiiTASInputWindow.h"
#include "Common/CommonTypes.h"
#include "Common/FileUtil.h"
#include "Core/Core.h"
#include "Core/HW/Wiimote.h"
#include "Core/HW/WiimoteEmu/Attachment/Classic.h"
#include "Core/HW/WiimoteEmu/Attachment/Nunchuk.h"
#include "Core/HW/WiimoteEmu/WiimoteEmu.h"
#include "Core/HW/WiimoteReal/WiimoteReal.h"
#include "DolphinQt2/TAS/Shared.h"
#include "InputCommon/InputConfig.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <algorithm>

WiiTASInputWindow::WiiTASInputWindow(QWidget* parent, int num) : QDialog(parent)
{
  m_num = num;
  m_ir_box = CreateStickInputs(this, tr("IR (ALT+F/G)"), &m_ir_x_value, &m_ir_y_value,
                               1024,  // TODO: Is the value 1024 a bug in wx?
                               768, Qt::Key_F, Qt::Key_G);
  m_nunchuk_stick_box =
      CreateStickInputs(this, tr("Nunchuk Stick (ALT+X/Y)"), &m_nunchuk_stick_x_value,
                        &m_nunchuk_stick_y_value, 255, 255, Qt::Key_X, Qt::Key_Y);

  m_classic_left_stick_box = CreateStickInputs(this, tr("Left Stick (ALT+F/G)"), &m_ir_x_value,
                                               &m_ir_y_value, 63, 63, Qt::Key_F, Qt::Key_G);

  m_classic_right_stick_box =
      CreateStickInputs(this, tr("Right Stick (ALT+Q/W)"), &m_classic_left_stick_value,
                        &m_classic_right_stick_value, 31, 31, Qt::Key_Q, Qt::Key_W);

  // Need to enforce the same minimum width because otherwise the different lengths in the labels
  // used on the QGroupBox will cause the StickWidgets to have different sizes.
  m_ir_box->setMinimumWidth(20);
  m_nunchuk_stick_box->setMinimumWidth(20);

  m_remote_orientation_box = new QGroupBox(tr("Remote Orientation"));

  auto* top_layout = new QHBoxLayout;
  top_layout->addWidget(m_ir_box);
  top_layout->addWidget(m_nunchuk_stick_box);
  top_layout->addWidget(m_classic_left_stick_box);
  top_layout->addWidget(m_classic_right_stick_box);

  auto* remote_orientation_x_layout =
      CreateSliderValuePairLayout(this, tr("X (ALT+Q)"), &m_remote_orientation_x_value, 1023,
                                  Qt::Key_Q, m_remote_orientation_box);
  auto* remote_orientation_y_layout =
      CreateSliderValuePairLayout(this, tr("Y (ALT+W)"), &m_remote_orientation_y_value, 1023,
                                  Qt::Key_W, m_remote_orientation_box);
  auto* remote_orientation_z_layout =
      CreateSliderValuePairLayout(this, tr("Z (ALT+E)"), &m_remote_orientation_z_value, 1023,
                                  Qt::Key_E, m_remote_orientation_box);

  auto* remote_orientation_layout = new QVBoxLayout;
  remote_orientation_layout->addLayout(remote_orientation_x_layout);
  remote_orientation_layout->addLayout(remote_orientation_y_layout);
  remote_orientation_layout->addLayout(remote_orientation_z_layout);
  m_remote_orientation_box->setLayout(remote_orientation_layout);

  m_nunchuk_orientation_box = new QGroupBox(tr("Nunchuk Orientation"));

  auto* nunchuk_orientation_x_layout =
      CreateSliderValuePairLayout(this, tr("X (ALT+I)"), &m_nunchuk_orientation_x_value, 1023,
                                  Qt::Key_I, m_nunchuk_orientation_box);
  auto* nunchuk_orientation_y_layout =
      CreateSliderValuePairLayout(this, tr("Y (ALT+O)"), &m_nunchuk_orientation_y_value, 1023,
                                  Qt::Key_O, m_nunchuk_orientation_box);
  auto* nunchuk_orientation_z_layout =
      CreateSliderValuePairLayout(this, tr("Z (ALT+P)"), &m_nunchuk_orientation_z_value, 1023,
                                  Qt::Key_P, m_nunchuk_orientation_box);

  auto* nunchuk_orientation_layout = new QVBoxLayout;
  nunchuk_orientation_layout->addLayout(nunchuk_orientation_x_layout);
  nunchuk_orientation_layout->addLayout(nunchuk_orientation_y_layout);
  nunchuk_orientation_layout->addLayout(nunchuk_orientation_z_layout);
  m_nunchuk_orientation_box->setLayout(nunchuk_orientation_layout);

  m_triggers_box = new QGroupBox(tr("Triggers"));
  auto* l_trigger_layout = CreateSliderValuePairLayout(this, tr("Left (ALT+N)"), &m_l_trigger_value,
                                                       31, Qt::Key_N, m_triggers_box);
  auto* r_trigger_layout = CreateSliderValuePairLayout(
      this, tr("Right (ALT+M)"), &m_l_trigger_value, 31, Qt::Key_M, m_triggers_box);

  auto* triggers_layout = new QVBoxLayout;
  triggers_layout->addLayout(l_trigger_layout);
  triggers_layout->addLayout(r_trigger_layout);
  m_triggers_box->setLayout(triggers_layout);

  m_a_button = new QCheckBox(QStringLiteral("&A"));
  m_b_button = new QCheckBox(QStringLiteral("&B"));
  m_1_button = new QCheckBox(QStringLiteral("&1"));
  m_2_button = new QCheckBox(QStringLiteral("&2"));
  m_plus_button = new QCheckBox(QStringLiteral("&+"));
  m_minus_button = new QCheckBox(QStringLiteral("&-"));
  m_home_button = new QCheckBox(QStringLiteral("&HOME"));
  m_left_button = new QCheckBox(QStringLiteral("&Left"));
  m_up_button = new QCheckBox(QStringLiteral("&Up"));
  m_down_button = new QCheckBox(QStringLiteral("&Down"));
  m_right_button = new QCheckBox(QStringLiteral("&Right"));
  m_c_button = new QCheckBox(QStringLiteral("&C"));
  m_z_button = new QCheckBox(QStringLiteral("&Z"));

  auto* buttons_layout1 = new QHBoxLayout;
  buttons_layout1->addWidget(m_a_button);
  buttons_layout1->addWidget(m_b_button);
  buttons_layout1->addWidget(m_1_button);
  buttons_layout1->addWidget(m_2_button);
  buttons_layout1->addWidget(m_plus_button);
  buttons_layout1->addWidget(m_minus_button);

  auto* buttons_layout2 = new QHBoxLayout;
  buttons_layout2->addWidget(m_home_button);
  buttons_layout2->addWidget(m_left_button);
  buttons_layout2->addWidget(m_up_button);
  buttons_layout2->addWidget(m_down_button);
  buttons_layout2->addWidget(m_right_button);

  auto* remote_buttons_layout = new QVBoxLayout;
  remote_buttons_layout->setSizeConstraint(QLayout::SetFixedSize);
  remote_buttons_layout->addLayout(buttons_layout1);
  remote_buttons_layout->addLayout(buttons_layout2);

  m_remote_buttons_box = new QGroupBox(tr("Remote Buttons"));
  m_remote_buttons_box->setLayout(remote_buttons_layout);

  auto* nunchuk_buttons_layout = new QHBoxLayout;
  nunchuk_buttons_layout->addWidget(m_c_button);
  nunchuk_buttons_layout->addWidget(m_z_button);

  m_nunchuk_buttons_box = new QGroupBox(tr("Nunchuk Buttons"));
  m_nunchuk_buttons_box->setLayout(nunchuk_buttons_layout);

  m_classic_a_button = new QCheckBox(QStringLiteral("&A"));
  m_classic_b_button = new QCheckBox(QStringLiteral("&B"));
  m_classic_x_button = new QCheckBox(QStringLiteral("&X"));
  m_classic_y_button = new QCheckBox(QStringLiteral("&Y"));
  m_classic_l_button = new QCheckBox(QStringLiteral("&L"));
  m_classic_r_button = new QCheckBox(QStringLiteral("&R"));
  m_classic_zl_button = new QCheckBox(QStringLiteral("&ZL"));
  m_classic_zr_button = new QCheckBox(QStringLiteral("ZR"));
  m_classic_plus_button = new QCheckBox(QStringLiteral("&+"));
  m_classic_minus_button = new QCheckBox(QStringLiteral("&-"));
  m_classic_home_button = new QCheckBox(QStringLiteral("&HOME"));
  m_classic_left_button = new QCheckBox(QStringLiteral("L&eft"));
  m_classic_up_button = new QCheckBox(QStringLiteral("&Up"));
  m_classic_down_button = new QCheckBox(QStringLiteral("&Down"));
  m_classic_right_button = new QCheckBox(QStringLiteral("R&ight"));

  auto* classic_buttons_layout1 = new QHBoxLayout;
  classic_buttons_layout1->addWidget(m_classic_a_button);
  classic_buttons_layout1->addWidget(m_classic_b_button);
  classic_buttons_layout1->addWidget(m_classic_x_button);
  classic_buttons_layout1->addWidget(m_classic_y_button);
  classic_buttons_layout1->addWidget(m_classic_l_button);
  classic_buttons_layout1->addWidget(m_classic_r_button);
  classic_buttons_layout1->addWidget(m_classic_zl_button);
  classic_buttons_layout1->addWidget(m_classic_zr_button);

  auto* classic_buttons_layout2 = new QHBoxLayout;
  classic_buttons_layout2->addWidget(m_classic_plus_button);
  classic_buttons_layout2->addWidget(m_classic_minus_button);
  classic_buttons_layout2->addWidget(m_classic_home_button);
  classic_buttons_layout2->addWidget(m_classic_left_button);
  classic_buttons_layout2->addWidget(m_classic_up_button);
  classic_buttons_layout2->addWidget(m_classic_down_button);
  classic_buttons_layout2->addWidget(m_classic_right_button);

  auto* classic_buttons_layout = new QVBoxLayout;
  classic_buttons_layout->setSizeConstraint(QLayout::SetFixedSize);
  classic_buttons_layout->addLayout(classic_buttons_layout1);
  classic_buttons_layout->addLayout(classic_buttons_layout2);

  m_classic_buttons_box = new QGroupBox(tr("Classic Buttons"));
  m_classic_buttons_box->setLayout(classic_buttons_layout);

  auto* layout = new QVBoxLayout;
  layout->addLayout(top_layout);
  layout->addWidget(m_remote_orientation_box);
  layout->addWidget(m_nunchuk_orientation_box);
  layout->addWidget(m_triggers_box);
  layout->addWidget(m_remote_buttons_box);
  layout->addWidget(m_nunchuk_buttons_box);
  layout->addWidget(m_classic_buttons_box);
  layout->setAlignment(m_nunchuk_buttons_box, Qt::AlignLeft);

  setLayout(layout);

  u8 ext = 0;
  if (Core::IsRunning())
  {
    ext = static_cast<WiimoteEmu::Wiimote*>(Wiimote::GetConfig()->GetController(num))
              ->CurrentExtension();
  }
  else
  {
    IniFile ini;
    ini.Load(File::GetUserPath(D_CONFIG_IDX) + "WiimoteNew.ini");
    std::string extension;
    ini.GetIfExists("Wiimote" + std::to_string(num + 1), "Extension", &extension);

    if (extension == "Nunchuk")
      ext = 1;
    if (extension == "Classic")
      ext = 2;
  }
  UpdateExt(ext);
}

void WiiTASInputWindow::UpdateExt(u8 ext)
{
  if (ext == 1)
  {
    setWindowTitle(tr("Wii TAS Input %1 - Remote + Nunchuk").arg(m_num + 1));
    m_ir_box->show();
    m_nunchuk_stick_box->show();
    m_classic_right_stick_box->hide();
    m_classic_left_stick_box->hide();
    m_remote_orientation_box->show();
    m_nunchuk_orientation_box->show();
    m_triggers_box->hide();
    m_nunchuk_buttons_box->show();
    m_remote_buttons_box->show();
    m_classic_buttons_box->hide();
  }
  else if (ext == 2)
  {
    setWindowTitle(tr("Wii TAS Input %1 - Classic Controller").arg(m_num + 1));
    m_ir_box->hide();
    m_nunchuk_stick_box->hide();
    m_classic_right_stick_box->show();
    m_classic_left_stick_box->show();
    m_remote_orientation_box->hide();
    m_nunchuk_orientation_box->hide();
    m_triggers_box->show();
    m_remote_buttons_box->hide();
    m_nunchuk_buttons_box->hide();
    m_classic_buttons_box->show();
  }
  else
  {
    setWindowTitle(tr("Wii TAS Input %1 - Remote").arg(m_num + 1));
    m_ir_box->show();
    m_nunchuk_stick_box->hide();
    m_classic_right_stick_box->hide();
    m_classic_left_stick_box->hide();
    m_remote_orientation_box->show();
    m_nunchuk_orientation_box->hide();
    m_triggers_box->hide();
    m_remote_buttons_box->show();
    m_nunchuk_buttons_box->hide();
    m_classic_buttons_box->hide();
  }
}
