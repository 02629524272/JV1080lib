// system_slots.cpp
/* Contains:
 * on_System_Upload_button_clicked
 * on_System_Upload_button_clicked
 * on_System_SaveData_button_clicked
 * on_System_LoadData_button_clicked
 * on_SysMode_select_currentIndexChanged
 * on_SysPerfSelect_currentIndexChanged
 * on_SysPatchSelect_currentIndexChanged
 * on_System_Sync_button_clicked
 * slotSysSetPerformanceMode
 * slotSysSetPatchMode
 * slotSysSetGmMode
 * on_EFX_switch_stateChanged
 * on_Chorus_switch_stateChanged
 * on_Reverb_switch_stateChanged
 * on_PatchRemain_switch_stateChanged
 * on_AcceptProgramChg_switch_stateChanged
 * on_AcceptBankSel_switch_stateChanged
 * on_AcceptControlChange_switch_stateChanged
 * on_AcceptModulation_switch_stateChanged
 * on_AcceptVolumeChg_switch_stateChanged
 * on_AcceptHold1Chg_switch_stateChanged
 * on_AcceptBender_switch_stateChanged
 * on_AcceptAftertouch_switch_stateChanged
 * on_SysClock_select_currentIndexChanged
 * on_SysTapControl_select_currentIndexChanged
 * on_SysHoldControl_select_currentIndexChanged
 * on_SysPeakControl_select_currentIndexChanged
 * on_SysVolumeControl_select_currentIndexChanged
 * on_SysAftertouchControl_select_currentIndexChanged
 * on_SysPatchRecvChannel_select_valueChanged
 * on_SysControlRecvChannel_select_valueChanged
 * on_SysPreviewMode_switch_toggled
 * on_SysPreviewNote1_select_valueChanged
 * on_SysPreviewNote2_select_valueChanged
 * on_SysPreviewNote3_select_valueChanged
 * on_SysPreviewNote4_select_valueChanged
 * on_SysPreviewNote1_volume_valueChanged
 * on_SysPreviewNote2_volume_valueChanged
 * on_SysPreviewNote3_volume_valueChanged
 * on_SysPreviewNote4_volume_valueChanged
 * on_SysTestTone_button_clicked
 * on_SysPanic_button_clicked
 * on_SysPerfNumber_valueChanged
 * on_SysPatchNumber_valueChanged
 * setPerfTabs
 * setPatchTabs
 * System_Loaded
 */

#include	"JVlibForm.h"
#include	"system_slots.h"
#include	<QtGui>
#include	<QDebug>

// Slots
void JVlibForm::on_System_Upload_button_clicked() {
  // NOTE: tbd

}
void JVlibForm::on_System_SaveData_button_clicked() {
  // call the Menu option for File/Save
  save();
}
void JVlibForm::on_System_LoadData_button_clicked() {
  // call the Menu option for File/Open
  open();
}

void JVlibForm::on_SysMode_select_currentIndexChanged(int val) {
  // called by SIGNAL when SysMode switch changes
  switch(val) {
    case 0:	// Performance mode
    if (state_table->updates_enabled) {
      system_area->sys_common.panel_mode=val;
      setSysSingleValue(addr_sys_panel_mode,val);
    }	// end UPDATES_ENABLED
    slotSysSetPerformanceMode();
    break;
    case 1:	// Patch mode
    if (state_table->updates_enabled) {
      system_area->sys_common.panel_mode=val;
      setSysSingleValue(addr_sys_panel_mode,val);
    }	// end UPDATES_ENABLED
    slotSysSetPatchMode();
    break;
    case 2:	// GM mode
    slotSysSetGmMode();
    break;
  }	// end SWITCH
}	// end on_SysMode_select_currentIndexChanged

void JVlibForm::on_SysPerfSelect_currentIndexChanged() {
  // called by SIGNAL when the SysPerf group is changed
  // or from the Perf Tab when those values are changed.
   if (state_table->updates_enabled) {
    system_area->sys_common.perf_num = SysPerfNumber->value() - 1;
  }	// end UPDATES_ENABLED
  if (state_table->jv_connect) {
    // update JV
    // Bank Select MSB
    change_3(0xB0+system_area->sys_common.control_channel, 0, SysPerfSelect->currentIndex()==0?0x50:0x51);
    // Bank Select LSB  
    change_3(0xB0+system_area->sys_common.control_channel, 0x20, SysPerfSelect->currentIndex()==2?0x01:0x00);
    // Program Change - Performance number is required
    change_2(0xC0+system_area->sys_common.control_channel, system_area->sys_common.perf_num);
    // get the new Performance name, update the Perf and Parts Tabs
  getSysPerfName();
  }	// end state_table->jv_connect
  if (MainTabWidget->currentIndex() == 0) {
    PerfGroup_select->setCurrentIndex(SysPerfSelect->currentIndex());
    PerfNumber_select->setValue(SysPerfNumber->value());
  } // end if(maintablewidget)
  PartsPerfGroup_display->setText(SysPerfSelect->currentText());
  PartsPerfNumber_display->setText(QString::number(SysPerfNumber->value()));
  EnablePerf(false);
  setPatchTabs(false);
}	// end slotSysSetPerf

void JVlibForm::on_SysPatchSelect_currentIndexChanged() {
  // called by SIGNAL when the Patch group or number is changed
  // System Tab display in Patch Mode
 if (state_table->updates_enabled) {
  int MSB,LSB;
  // update sys_common.patch_num_high/low
  int Hval = ((SysPatchNumber->value()-1)/16);
  int Lval = ((SysPatchNumber->value()-1)%16);
  system_area->sys_common.patch_num_high = Hval;
  system_area->sys_common.patch_num_low = Lval;
  // update sys_common.patch_group and group_id
  switch(SysPatchSelect->currentIndex()) {
    case 0:	// User
      system_area->sys_common.patch_group = 0x00;
      system_area->sys_common.patch_group_id = 0x01;
      MSB = 0x50;
      LSB = 0x0;
      break;
    case 1:	// Exp A
      system_area->sys_common.patch_group = 0x02;
      system_area->sys_common.patch_group_id = 0x02;
      MSB = 0x54;
      LSB = Hval>7?1:0;
      break;
    case 2:	// PresetA
      system_area->sys_common.patch_group = 0x00;
      system_area->sys_common.patch_group_id = 0x03;
      MSB = 0x51;
      LSB = 0x0;
      break;
    case 3:	// PresetB
      system_area->sys_common.patch_group = 0x00;
      system_area->sys_common.patch_group_id = 0x04;
      MSB = 0x51;
      LSB = 0x1;
      break;
    case 4:	// PresetC
      system_area->sys_common.patch_group = 0x00;
      system_area->sys_common.patch_group_id = 0x05;
      MSB = 0x51;
      LSB = 0x2;
      break;
    case 5:	// PresetD
      system_area->sys_common.patch_group = 0x00;
      system_area->sys_common.patch_group_id = 0x06;
      MSB = 0x51;
      LSB = 0x3;
      break;
    case 6:	// Exp B
      system_area->sys_common.patch_group = 0x02;
      system_area->sys_common.patch_group_id = 0x10;
      MSB = 0x54;
      LSB = Hval>7?3:2;
      break;
    case 7:	// Exp C
      system_area->sys_common.patch_group = 0x02;
      system_area->sys_common.patch_group_id = 0x62;
      MSB = 0x54;
      LSB = Hval>7?5:4;
      break;
    default:
      system_area->sys_common.patch_group = 0x00;
      system_area->sys_common.patch_group_id = 0x00;
      MSB = 0x00;
      LSB = 0x00;
      break;
  }
  //  Set Patch Max
  switch(SysPatchSelect->currentIndex()) {
    case 0:	// User
      SysPatchNumber->setMaximum(128);
      break;
    case 1:	// Exp A
      SysPatchNumber->setMaximum(255);
      break;
    case 2:	// PresetA
      SysPatchNumber->setMaximum(128);
      break;
    case 3:	// PresetB
      SysPatchNumber->setMaximum(128);
      break;
    case 4:	// PresetC
      SysPatchNumber->setMaximum(128);
      break;
    case 5:	// PresetD
      SysPatchNumber->setMaximum(128);
      break;
    case 6:	// Exp B
      SysPatchNumber->setMaximum(256);
      break;
    case 7:	// Exp C
      SysPatchNumber->setMaximum(100);
      break;
    default:
      SysPatchNumber->setMaximum(128);
      break;
  }
  if (state_table->jv_connect) {
    // update JV
    change_3(0xB0+system_area->sys_common.patch_receive_channel, 0x0, MSB);
    // Bank Select LSB  
    change_3(0xB0+system_area->sys_common.patch_receive_channel, 0x20, LSB);
    // Program Change - Performance number
    change_2(0xC0+system_area->sys_common.patch_receive_channel, SysPatchNumber->value()>128?SysPatchNumber->value()-129 :SysPatchNumber->value()-1);
    getSysPatchName();
  }	// end state_table->jv_connect
  // get the current Patch Mode patch name, update other Tabs as needed
  Patch_Group_select->blockSignals(true);
  Patch_Group_select->setCurrentIndex(SysPatchSelect->currentIndex());
  Patch_Group_select->blockSignals(false);
  Patch_Number_select->blockSignals(true);
  Patch_Number_select->setValue(SysPatchNumber->value());
  Patch_Number_select->blockSignals(false);
  Patch_Name_edit->blockSignals(true);
  Patch_Name_edit->setText(SysPatchName->text());
  Patch_Name_edit->blockSignals(false);
  Patch_Group_select->setEnabled(AcceptBankSel_switch->isChecked());
  Patch_Number_select->setEnabled(AcceptProgramChg_switch->isChecked());
  Patch_Name_edit->setEnabled(true);
  PatchEFX_Group_display->setEnabled(true);
  PatchEFX_Number_display->setEnabled(true);
  PatchEFX_Name_display->setEnabled(true);
  PatchEFX_Group_display->setText(SysPatchSelect->currentText());
  PatchEFX_Number_display->setText(QString::number(SysPatchNumber->value()));
  PatchEFX_Name_display->setText(Patch_Name_edit->text());
  }	// end UPDATES_ENABLED
  EnablePatch(false);
  setPerfTabs(false);
  MainTabWidget->setTabEnabled(3,true);	// Patch tab
  state_table->patchTab_enable = true;    
}	// end slotSysSetPatch

int JVlibForm::on_System_Sync_button_clicked() {  
  if (!strlen(MIDI_dev)) {
    QMessageBox::critical(this, "JVlib", tr("No MIDI port selected"));
    return EXIT_FAILURE;
  }
  unsigned char	buf[8];
  int	Stop=0;
  // get System common
  memset(buf,0,sizeof(buf));
  // get system_area common
  for (int x=0;x<4;x++) buf[x]=system_common_addr[x];
  for (int x=4;x<8;x++) buf[x]=system_common_size[x-4];
  // open the selected midi port
  if (open_ports() == EXIT_FAILURE) return EXIT_FAILURE;
  RetryA:
  if (sysex_request(buf,8) == EXIT_FAILURE) { close_ports(); return EXIT_FAILURE; }
  int err = sysex_get((unsigned char *)&system_area->sys_common.panel_mode, (char *)system_common_size);
  if (err == EXIT_FAILURE) { close_ports(); return EXIT_FAILURE; }
  if (err==2 && Stop<MAX_RETRIES) { if (debug) qDebug() << "Retrying"; Stop++; sleep(1*Stop); goto RetryA; }
  if (err==3 && Stop<MAX_RETRIES) { if (debug) qDebug() << "Retrying"; Stop++; sleep(1*Stop); goto RetryA; }
  if (err != EXIT_SUCCESS) { close_ports(); puts("Try GM mode?"); return EXIT_FAILURE; }
  close_ports();
  setSystemParms();
  EnableSys(true);
  action_Offline->setChecked(false);
  System_JV_status->on();
  MainTabWidget->setTabEnabled(11,true);	// enable Tuning tab
  state_table->tuningTab_enable = true;
  if (state_table->perf_mode) {
    slotSysSetPerformanceMode();
  }
  if (state_table->patch_mode) {
    slotSysSetPatchMode();
  }
  statusbar->showMessage("System loaded",0);
  SysMode_select->setFocus();
  return EXIT_SUCCESS;
}	// end on_System_Sync_button_clicked

void JVlibForm::slotSysSetPerformanceMode() {
  // called when slotSysSetMode switch changes
    MainTabWidget->setTabEnabled(1,true);	// Performance tab
    state_table->performanceTab_enable = true;
    setPerfTabs(false);
    MainTabWidget->setTabEnabled(3,false);	// Patch tab
    state_table->patchTab_enable = false;
    SysControlRecvChannel_select->setEnabled(true);
    SysPerformance_box->setEnabled(true);
    SysPerfSelect->setEnabled(AcceptBankSel_switch->isChecked());
    SysPerfNumber->setEnabled(AcceptProgramChg_switch->isChecked());
    if (state_table->jv_connect && state_table->updates_enabled) {
      getSysPerfName();
    }
    SysPatch_box->setEnabled(false);
    SysPatchRecvChannel_select->setEnabled(false);
    PerfSync_button->setEnabled(true);
    PerfGroup_select->setEnabled(AcceptBankSel_switch->isChecked());
    PerfNumber_select->setEnabled(AcceptProgramChg_switch->isChecked());
    PerfName_edit->setEnabled(true);
    Patch_Sync_button->setEnabled(false);
    Patch_PerfPartNum_select->blockSignals(true);
    if (Patch_PerfPartNum_select->itemText(0)=="0")
      Patch_PerfPartNum_select->removeItem(0);
    Patch_PerfPartNum_select->blockSignals(false);
    EnablePerf(false);
    EnablePatch(false);
    setSysGmMode(false);
    setPatchTabs(false);
    state_table->perf_mode = true;
    state_table->patch_mode = false;
    state_table->GM_mode = false;
    Tuning_Sync_status->off();
    state_table->tuning_sync = false;
    state_table->tuning_modified = false;
}	// end slotSysSetPerformanceMode

void JVlibForm::slotSysSetPatchMode() {
  // called when slotSysSetMode switch changes
    if (state_table->jv_connect && state_table->updates_enabled)
      getSysPatchName();
    setPerfTabs(false);
    PerfGroup_select->setEnabled(false);
    PerfNumber_select->setEnabled(false);
    PerfName_edit->setEnabled(false);
    SysPerformance_box->setEnabled(false);
    SysPatch_box->setEnabled(true);
    SysPatchSelect->setEnabled(AcceptBankSel_switch->isChecked());
    SysPatchNumber->setEnabled(AcceptProgramChg_switch->isChecked());
    SysPatchRecvChannel_select->setEnabled(true);
    SysControlRecvChannel_select->setEnabled(false);
    MainTabWidget->setTabEnabled(1,false);	// Performance tab
    state_table->performanceTab_enable = false;
    setSysGmMode(false);
    EnablePerf(false);
    state_table->perf_mode = false;
    state_table->patch_mode = true;
    state_table->GM_mode = false;
    state_table->updates_enabled = false;
    if (Patch_PerfPartNum_select->itemText(0)=="1")
      Patch_PerfPartNum_select->insertItem(0,"0");
    Patch_PerfPartNum_select->setCurrentIndex(0);
    EnablePatch(false);
    Patch_Group_select->setEnabled(AcceptBankSel_switch->isChecked());
    Patch_Number_select->setEnabled(AcceptProgramChg_switch->isChecked());
    Patch_Name_edit->setEnabled(true);
    Patch_Sync_button->setEnabled(true);
    setPatchTabs(false);
    MainTabWidget->setTabEnabled(3,true);	// Patch tab
    state_table->patchTab_enable = true;    
    state_table->updates_enabled = true;
    Tuning_Sync_status->off();
    state_table->tuning_sync = false;
    state_table->tuning_modified = false;
//    Tuning_ScaleTuning_enable->setChecked(false);
//    Tuning_ScaleTuning_enable->setEnabled(false);
}	// end slotSysSetPatchMode

void JVlibForm::slotSysSetGmMode() {
  // called when slotSysSetMode switch changes
  if (state_table->updates_enabled) {
  int reply = QMessageBox::critical(this,"JVlib", 
    QString::fromAscii("Changing to GM Mode will lock this application in that mode!\nYou will need to manually reset the JV-1080 front panel to change modes again.\nAre you sure you want to proceed?"),
    QMessageBox::Ok | QMessageBox::Cancel);
  if (reply == QMessageBox::Cancel) {
    SysMode_select->setCurrentIndex(system_area->sys_common.panel_mode);
    return;
  }
  if (state_table->updates_enabled) {
   system_area->sys_common.panel_mode=2;
   setSysSingleValue(addr_sys_panel_mode,2);
  }	// end UPDATES_ENABLED
  state_table->perf_mode = false;
  state_table->patch_mode = false;
  state_table->GM_mode = true;
  setSysGmMode(true);
  }	// end state_table->updates_enabled
}	// end slotSysSetGmMode

// --------------------------------------------------------------------------------------------------------------------
// update memory and the synth when controls are changed
void JVlibForm::on_EFX_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.EFX_switch = (val==Qt::Checked?true:false);
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_EFX_switch,system_area->sys_common.EFX_switch);
  } 
}
void JVlibForm::on_Chorus_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.chorus_switch = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_chorus_switch,system_area->sys_common.chorus_switch);
  }
}
void JVlibForm::on_Reverb_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.reverb_switch = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_reverb_switch,system_area->sys_common.reverb_switch);
  } 
}
void JVlibForm::on_PatchRemain_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.patch_remain = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_patch_remain,system_area->sys_common.patch_remain);
  }
}
void JVlibForm::on_AcceptProgramChg_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_program_change = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_program_change,system_area->sys_common.receive_program_change);
  if (state_table->perf_mode) {
    SysPerfNumber->setEnabled(val);
    if (state_table->performance_sync) {
      PerfNumber_select->setEnabled(val);
      Part1_ReceivePrgChg_enable->setEnabled(val);
      Part2_ReceivePrgChg_enable->setEnabled(val);
      Part3_ReceivePrgChg_enable->setEnabled(val);
      Part4_ReceivePrgChg_enable->setEnabled(val);
      Part5_ReceivePrgChg_enable->setEnabled(val);
      Part6_ReceivePrgChg_enable->setEnabled(val);
      Part7_ReceivePrgChg_enable->setEnabled(val);
      Part8_ReceivePrgChg_enable->setEnabled(val);
      Part9_ReceivePrgChg_enable->setEnabled(val);
      Part10_ReceivePrgChg_enable->setEnabled(val);
      Part11_ReceivePrgChg_enable->setEnabled(val);
      Part12_ReceivePrgChg_enable->setEnabled(val);
      Part13_ReceivePrgChg_enable->setEnabled(val);
      Part14_ReceivePrgChg_enable->setEnabled(val);
      Part15_ReceivePrgChg_enable->setEnabled(val);
      Part16_ReceivePrgChg_enable->setEnabled(val);
      Part1_PatchNumber_select->setEnabled(val && Part1_ReceivePrgChg_enable->isChecked());
      Part2_PatchNumber_select->setEnabled(val && Part2_ReceivePrgChg_enable->isChecked());
      Part3_PatchNumber_select->setEnabled(val && Part3_ReceivePrgChg_enable->isChecked());
      Part4_PatchNumber_select->setEnabled(val && Part4_ReceivePrgChg_enable->isChecked());
      Part5_PatchNumber_select->setEnabled(val && Part5_ReceivePrgChg_enable->isChecked());
      Part6_PatchNumber_select->setEnabled(val && Part6_ReceivePrgChg_enable->isChecked());
      Part7_PatchNumber_select->setEnabled(val && Part7_ReceivePrgChg_enable->isChecked());
      Part8_PatchNumber_select->setEnabled(val && Part8_ReceivePrgChg_enable->isChecked());
      Part9_PatchNumber_select->setEnabled(val && Part9_ReceivePrgChg_enable->isChecked());
      Part10_PatchNumber_select->setEnabled(val && Part10_ReceivePrgChg_enable->isChecked());
      Part11_PatchNumber_select->setEnabled(val && Part11_ReceivePrgChg_enable->isChecked());
      Part12_PatchNumber_select->setEnabled(val && Part12_ReceivePrgChg_enable->isChecked());
      Part13_PatchNumber_select->setEnabled(val && Part13_ReceivePrgChg_enable->isChecked());
      Part14_PatchNumber_select->setEnabled(val && Part14_ReceivePrgChg_enable->isChecked());
      Part15_PatchNumber_select->setEnabled(val && Part15_ReceivePrgChg_enable->isChecked());
      Part16_PatchNumber_select->setEnabled(val && Part16_ReceivePrgChg_enable->isChecked());
    }
    if (state_table->rhythm_sync) Rhythm_WaveNumber_select->setEnabled(val);
  }
  if (state_table->patch_mode) {
    SysPatchNumber->setEnabled(val);
    if (state_table->patch_sync) Patch_Number_select->setEnabled(val && AcceptProgramChg_switch->isChecked());
  }
  if (state_table->tone_sync) {
    Tone_Number_select->setEnabled(val);
  }
  Patch_Number_select->setEnabled(val && AcceptProgramChg_switch->isChecked());
  }
}

void JVlibForm::on_AcceptBankSel_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_bank_select = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_bank_select,system_area->sys_common.receive_bank_select);
  }
  if (state_table->perf_mode) {
    SysPerfSelect->setEnabled(val);
    PerfGroup_select->setEnabled(val);
    if (state_table->performance_sync) {
      Part1_PatchGroup_select->setEnabled(val && Part1_ReceivePrgChg_enable->isChecked());
      Part2_PatchGroup_select->setEnabled(val && Part2_ReceivePrgChg_enable->isChecked());
      Part3_PatchGroup_select->setEnabled(val && Part3_ReceivePrgChg_enable->isChecked());
      Part4_PatchGroup_select->setEnabled(val && Part4_ReceivePrgChg_enable->isChecked());
      Part5_PatchGroup_select->setEnabled(val && Part5_ReceivePrgChg_enable->isChecked());
      Part6_PatchGroup_select->setEnabled(val && Part6_ReceivePrgChg_enable->isChecked());
      Part7_PatchGroup_select->setEnabled(val && Part7_ReceivePrgChg_enable->isChecked());
      Part8_PatchGroup_select->setEnabled(val && Part8_ReceivePrgChg_enable->isChecked());
      Part9_PatchGroup_select->setEnabled(val && Part9_ReceivePrgChg_enable->isChecked());
      Part10_PatchGroup_select->setEnabled(val && Part10_ReceivePrgChg_enable->isChecked());
      Part11_PatchGroup_select->setEnabled(val && Part11_ReceivePrgChg_enable->isChecked());
      Part12_PatchGroup_select->setEnabled(val && Part12_ReceivePrgChg_enable->isChecked());
      Part13_PatchGroup_select->setEnabled(val && Part13_ReceivePrgChg_enable->isChecked());
      Part14_PatchGroup_select->setEnabled(val && Part14_ReceivePrgChg_enable->isChecked());
      Part15_PatchGroup_select->setEnabled(val && Part15_ReceivePrgChg_enable->isChecked());
      Part16_PatchGroup_select->setEnabled(val && Part16_ReceivePrgChg_enable->isChecked());
    }
    if (state_table->rhythm_sync) Rhythm_WaveGroup_select->setEnabled(val);
  }
  if (state_table->patch_mode) {
    SysPatchSelect->setEnabled(val);
    if (state_table->patch_sync) {
      Patch_Group_select->setEnabled(val && AcceptBankSel_switch->isChecked());
    }
  }
  if (state_table->tone_sync) {
    Tone_Group_select->setEnabled(val && AcceptBankSel_switch->isChecked());
  }
  Patch_Group_select->setEnabled(val && AcceptBankSel_switch->isChecked());
}

void JVlibForm::on_AcceptControlChange_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_control_change = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_control_change,system_area->sys_common.receive_control_change);
  } 
}
void JVlibForm::on_AcceptModulation_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_modulation = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_modulation,system_area->sys_common.receive_modulation);
  } 
}
void JVlibForm::on_AcceptVolumeChg_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_volume = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_volume,system_area->sys_common.receive_volume);
  Part1_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
  Part2_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
  Part3_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
  Part4_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
  Part5_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
  } 
}
void JVlibForm::on_AcceptHold1Chg_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_hold_1 = val==Qt::Checked?true:false;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_hold_1,system_area->sys_common.receive_hold_1);
  Part1_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  Part2_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  Part3_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  Part4_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  Part5_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  }
}
void JVlibForm::on_AcceptBender_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_bender = (val==Qt::Checked?true:false);
  Patch_BenderRange_box->setEnabled(val==Qt::Checked?true:false);
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_bender,system_area->sys_common.receive_bender);
  }
}
void JVlibForm::on_AcceptAftertouch_switch_stateChanged(int val) { 
  if (state_table->updates_enabled) {
  system_area->sys_common.receive_aftertouch = (val==Qt::Checked?true:false);
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_receive_aftertouch,system_area->sys_common.receive_aftertouch);
  }
}

void JVlibForm::on_SysClock_select_currentIndexChanged(int idx) {
  if (state_table->updates_enabled) {
  system_area->sys_common.clock_source = idx;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_clock_source, system_area->sys_common.clock_source);
  }
}
void JVlibForm::on_SysTapControl_select_currentIndexChanged(int idx) {
  if (state_table->updates_enabled) {
  system_area->sys_common.tap_control_source = idx;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_tap_control_source, system_area->sys_common.tap_control_source);
  }
}
void JVlibForm::on_SysHoldControl_select_currentIndexChanged(int idx) {
  if (state_table->updates_enabled) {
  system_area->sys_common.hold_control_source = idx;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_hold_control_source, system_area->sys_common.hold_control_source);
  }
}
void JVlibForm::on_SysPeakControl_select_currentIndexChanged(int idx) {
  if (state_table->updates_enabled) {
  system_area->sys_common.peak_control_source = idx;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_peak_control_source, system_area->sys_common.peak_control_source);
  }
}
void JVlibForm::on_SysVolumeControl_select_currentIndexChanged(int idx) {
  if (state_table->updates_enabled) {
  system_area->sys_common.volume_control_source = idx;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_volume_control_source, system_area->sys_common.volume_control_source);
  }
}
void JVlibForm::on_SysAftertouchControl_select_currentIndexChanged(int idx) {
  if (state_table->updates_enabled) {
  system_area->sys_common.aftertouch_control_source = idx;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_aftertouch_control_source, system_area->sys_common.aftertouch_control_source);
  }
}
void JVlibForm::on_SysPatchRecvChannel_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
  system_area->sys_common.patch_receive_channel = val-1;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_patch_receive_channel, system_area->sys_common.patch_receive_channel);
  }
}
void JVlibForm::on_SysControlRecvChannel_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
  system_area->sys_common.control_channel = val>0?val-1:16;
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_control_channel, system_area->sys_common.control_channel);
  }
}

void JVlibForm::on_SysPreviewMode_switch_toggled(bool checked) {
  if (state_table->updates_enabled) {
  // update system_area
  system_area->sys_common.preview_mode = checked;
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_mode, checked);
  // update button text
  SysPreviewMode_switch->setText( checked ? QString::fromUtf8("Chord") : QString::fromUtf8("Single") );
  }
}

void JVlibForm::on_SysPreviewNote1_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
  // update system_area
  system_area->sys_common.preview_key_set_1=val;
  // update display
  SysPreviewNote1_display->setText(funcNoteCalc(val));
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_key_1,val);
  }
}
void JVlibForm::on_SysPreviewNote2_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
  // update system_area
  system_area->sys_common.preview_key_set_2=val;
  // update display
  SysPreviewNote2_display->setText(funcNoteCalc(val));
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_key_2,val);
  }
}
void JVlibForm::on_SysPreviewNote3_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
  // update system_area
  system_area->sys_common.preview_key_set_3=val;
  // update display
  SysPreviewNote3_display->setText(funcNoteCalc(val));
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_key_3,val);
  }
}
void JVlibForm::on_SysPreviewNote4_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
  // update system_area
  system_area->sys_common.preview_key_set_4=val;
  // update display
  SysPreviewNote4_display->setText(funcNoteCalc(val));
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_key_4,val);
  }
}
void JVlibForm::on_SysPreviewNote1_volume_valueChanged(int val) {
  if (state_table->updates_enabled) {
  QString str;
  SysPreviewNote1_volume->setToolTip(str.setNum(val));
  // update system_area
  system_area->sys_common.preview_velocity_set_1=val;
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_velocity_1,val);
  }
}
void JVlibForm::on_SysPreviewNote2_volume_valueChanged(int val) {
  if (state_table->updates_enabled) {
  QString str;
  SysPreviewNote2_volume->setToolTip(str.setNum(val));
  // update system_area
  system_area->sys_common.preview_velocity_set_2=val;
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_velocity_2,val);
  }
}
void JVlibForm::on_SysPreviewNote3_volume_valueChanged(int val) {
  if (state_table->updates_enabled) {
  QString str;
  SysPreviewNote3_volume->setToolTip(str.setNum(val));
  // update system_area
  system_area->sys_common.preview_velocity_set_3=val;
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_velocity_3,val);
  }
}
void JVlibForm::on_SysPreviewNote4_volume_valueChanged(int val) {
  if (state_table->updates_enabled) {
  QString str;
  SysPreviewNote4_volume->setToolTip(str.setNum(val));
  // update system_area
  system_area->sys_common.preview_velocity_set_4=val;
  // update synth
  if (state_table->jv_connect)
    setSysSingleValue(addr_sys_preview_velocity_4,val);
  }
}

// --------------------------------------------------------------------------------------------------------------------
// Button actions on the System Tab
void JVlibForm::on_SysTestTone_button_clicked(bool val) {
 if (state_table->jv_connect) {
  if (val) {
    change_3(0x90, SysPreviewNote1_select->value(), SysPreviewNote1_volume->value());
  } 
  else {
    change_3(0xB0, 0x7B, 0x0);
    change_3(0xB0, 0x79, 0x0);
  }	// end else
  SysTestTone_button->setText(val ? QString::fromUtf8("Stop") : QString::fromUtf8("Test Tone") );
 }	// end jv_connect
}

void JVlibForm::on_SysPanic_button_clicked() {
  unsigned char buf[96];
  for (int x=0;x<96;x++) buf[x]=all_sounds_off[x];
  if (open_ports() == EXIT_FAILURE) return;
  if (change_send(buf,96) == EXIT_FAILURE) { close_ports(); return; }
  close_ports();
}

void JVlibForm::on_SysPerfNumber_valueChanged(int val) {
   if (state_table->updates_enabled) {
    system_area->sys_common.perf_num = val - 1;
    if (state_table->jv_connect) {
      // Program Change - Performance number
      change_2(0xC0+system_area->sys_common.control_channel, val-1);
      // get the new Performance name, update the Perf and Parts Tabs
      getSysPerfName();
    }	// end state_table->jv_connect
  }	// end UPDATES_ENABLED
  if (MainTabWidget->currentIndex() == 0)
    PerfNumber_select->setValue(SysPerfNumber->value());
  PartsPerfNumber_display->setText(QString::number(SysPerfNumber->value()));
  EnablePerf(false);
}	// end on_SysPerfNumber_valueChanged

void JVlibForm::on_SysPatchNumber_valueChanged(int val) {
  if (state_table->updates_enabled) {
    on_SysPatchSelect_currentIndexChanged();
/*    // update sys_common.patch_num_high/low
    int Hval = ((val-1)/16);
    int Lval = ((val-1)%16);
    system_area->sys_common.patch_num_high = Hval;
    system_area->sys_common.patch_num_low = Lval;
    if (state_table->jv_connect) {
      // Program Change - Patch number
      change_2(0xC0+system_area->sys_common.patch_receive_channel, val-1);
    // get the new Patch name, update the Patch Tab
    getSysPatchName();
    }	// end state_table->jv_connect
*/
  // get the current Patch Mode patch name, update other Tabs as needed
    state_table->updates_enabled = false;
    Patch_Group_select->setCurrentIndex(SysPatchSelect->currentIndex());
//  Patch_Number_select->setValue(SysPatchNumber->value());
    Patch_Number_select->setValue(val);
    Patch_Name_edit->setText(SysPatchName->text());
    state_table->updates_enabled = true;
    PatchEFX_Group_display->setText(SysPatchSelect->currentText());
//  PatchEFX_Number_display->setText(QString::number(SysPatchNumber->value()));
    PatchEFX_Number_display->setText(QString::number(val));
    PatchEFX_Name_display->setText(Patch_Name_edit->text());
  }	// end UPDATES_ENABLED
  EnablePatch(false);
  Patch_Group_select->setEnabled(AcceptBankSel_switch->isChecked());
  Patch_Number_select->setEnabled(AcceptProgramChg_switch->isChecked());
  Patch_Name_edit->setEnabled(true);
  PatchEFX_Group_display->setEnabled(true);
  PatchEFX_Number_display->setEnabled(true);
  PatchEFX_Name_display->setEnabled(true);
}	// end on_SysPatchNumber_valueChanged

void JVlibForm::setPerfTabs(bool val) {
  MainTabWidget->setTabEnabled(2,val);
  MainTabWidget->setTabEnabled(3,val);
  MainTabWidget->setTabEnabled(4,val);
  state_table->partsTab_enable = val;
  state_table->patchTab_enable = val;
  state_table->rhythmTab_enable = val;
}

void JVlibForm::setPatchTabs(bool val) {
  MainTabWidget->setTabEnabled(5,val);
  MainTabWidget->setTabEnabled(6,val);
  MainTabWidget->setTabEnabled(7,val);
  MainTabWidget->setTabEnabled(8,val);
  MainTabWidget->setTabEnabled(9,val);
  MainTabWidget->setTabEnabled(10,val);
  state_table->patchEFXTab_enable = val;
  state_table->toneTab_enable = val;
  state_table->toneEFXTab_enable = val;
  state_table->toneTVFTab_enable = val;
  state_table->toneTVATab_enable = val;
  state_table->pitchTab_enable = val;
}

void JVlibForm::System_Loaded() {
  setSystemParms();
}
