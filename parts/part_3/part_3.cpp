// part_3.cpp
/* Contains:
 * on_Part3_TestTone_switch_toggled
 * on_Part3_ReceiveMidi_enable_toggled
 * on_Part3_MidiChannel_select_valueChanged
 * on_Part3_Level_select_valueChanged
 * on_Part3_Pan_select_valueChanged
 * on_Part3_Transpose_select_valueChanged
 * on_Part3_TuneAdj_select_valueChanged
 * on_Part3_Output_select_currentIndexChanged
 * on_Part3_OutputLevel_select_valueChanged
 * on_Part3_ChorusSend_select_valueChanged
 * on_Part3_ReverbSend_select_valueChanged
 * on_Part3_ReceivePrgChg_enable_toggled
 * on_Part3_ReceiveVolume_enable_toggled
 * on_Part3_ReceiveHold_enable_toggled
 * on_Part3_LowLimit_select_valueChanged
 * on_Part3_HighLimit_select_valueChanged
 * on_Part3_VoiceReserve_select_valueChanged
 * on_Part3_PatchGroup_select_currentIndexChanged
 * Part3_SetPatchMax
 * on_Part3_PatchNumber_select_valueChanged
 */

#include        "JVlibForm.h"
#include        <QtGui>

void JVlibForm::on_Part3_TestTone_switch_toggled(bool val) {
  PartsToneSwitch(Part3_MidiChannel_select->value()-1, val);
  Part3_TestTone_switch->setText(val ? QString::fromUtf8("Stop") : QString::fromUtf8("Play Patch") );
}

//---------------------------------------------------------------------------------------------------------------------
// switches
void JVlibForm::on_Part3_ReceiveMidi_enable_toggled(bool val) {
  if (state_table->perf_mode) setPartSingleValue(2,0,val);
}
void JVlibForm::on_Part3_MidiChannel_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(2,1,val-1);
}
void JVlibForm::on_Part3_Level_select_valueChanged(int val) {
  Part3_Level_select->setStatusTip(QString::number(val));
  if (state_table->perf_mode) setPartSingleValue(2,6,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part3_MidiChannel_select->value()-1,0x07,val);
}
void JVlibForm::on_Part3_Pan_select_valueChanged(int val) {
//  Part3_Level_select->setStatusTip(QString::number(val));
  if (state_table->perf_mode) setPartSingleValue(2,7,val+64);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part3_MidiChannel_select->value()-1,0x0A,val+64);
}
void JVlibForm::on_Part3_Transpose_select_valueChanged(int val) { 
  // val is between -48,+48 half-steps
  if (state_table->perf_mode) setPartSingleValue(2, 0x8, val+48);
  // GM wants val between 10h,70h (16->112)
  if (state_table->GM_mode && state_table->updates_enabled) change_12(0xB0+Part3_MidiChannel_select->value()-1,0x65,0x00,0xB0+Part3_MidiChannel_select->value()-1,0x64,0x02,0xB0+Part3_MidiChannel_select->value()-1,0x06,(val+64),0xB0+Part3_MidiChannel_select->value()-1,0x26,0x00);
}
void JVlibForm::on_Part3_TuneAdj_select_valueChanged(int val) { 
  // val is between -50,+50 cents
  if (state_table->perf_mode) setPartSingleValue(2, 0x9, val+50);
  // GM wants val between 20h,60h (32->96)
  if (state_table->GM_mode && state_table->updates_enabled) change_12(0xB0+Part3_MidiChannel_select->value()-1,0x65,0x00,0xB0+Part3_MidiChannel_select->value()-1,0x64,0x01,0xB0+Part3_MidiChannel_select->value()-1,0x06,(val+50)*65/101+32,0xB0+Part3_MidiChannel_select->value()-1,0x26,0x00);
}
void JVlibForm::on_Part3_Output_select_currentIndexChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(2,0xA,val);
}
void JVlibForm::on_Part3_OutputLevel_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(2,0xB,val);
}
void JVlibForm::on_Part3_ChorusSend_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(2,0xC,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part3_MidiChannel_select->value()-1,0x5D,val);
}
void JVlibForm::on_Part3_ReverbSend_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(2,0xD,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part3_MidiChannel_select->value()-1,0x5B,val);
}
void JVlibForm::on_Part3_ReceivePrgChg_enable_toggled(bool val) {
  setPartSingleValue(2,0xE,val);
}
void JVlibForm::on_Part3_ReceiveVolume_enable_toggled(bool val) {
  if (state_table->perf_mode) setPartSingleValue(2,0xF,val);
}
void JVlibForm::on_Part3_ReceiveHold_enable_toggled(bool val) {
  if (state_table->perf_mode) setPartSingleValue(2,0x10,val);
}
void JVlibForm::on_Part3_LowLimit_select_valueChanged(int val) { 
  Part3_LowLimit_display->setText(funcNoteCalc(val));
  if (val>Part3_HighLimit_select->value()) on_Part3_HighLimit_select_valueChanged(val);
  setPartSingleValue(2, 0x11, val);
}
void JVlibForm::on_Part3_HighLimit_select_valueChanged(int val) { 
  Part3_HighLimit_display->setText(funcNoteCalc(val));
  if (val<Part3_LowLimit_select->value()) on_Part3_LowLimit_select_valueChanged(val);
  setPartSingleValue(2, 0x12, val);
}
void JVlibForm::on_Part3_VoiceReserve_select_valueChanged(int val) { 
  if (setVoiceCounters(3))
    setPerfSingleValue(0x30+2,val);
}

//---------------------------------------------------------------------------------------------------------------------
void JVlibForm::on_Part3_PatchGroup_select_currentIndexChanged(int val) {
  // called after a change in the Patch group or number for this part to update the Patch Name and active_area memory
  if (!state_table->updates_enabled) return;
  switch(val) {
    case 0:	// User patch
      Part3_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[2].patch_group = 0x0;
      active_area->active_performance.perf_part[2].patch_group_id = 0x01;
      break;
    case 1:	// Exp A
      Part3_PatchNumber_select->setMaximum(255);
      active_area->active_performance.perf_part[2].patch_group = 0x02;
      active_area->active_performance.perf_part[2].patch_group_id = 0x02;
      break;
    case 2:	// PresetA
      Part3_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[2].patch_group = 0x00;
      active_area->active_performance.perf_part[2].patch_group_id = 0x03;
      break;
    case 3:	// PresetB
      Part3_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[2].patch_group = 0x00;
      active_area->active_performance.perf_part[2].patch_group_id = 0x04;
      break;
    case 4:	// PresetC
      Part3_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[2].patch_group = 0x00;
      active_area->active_performance.perf_part[2].patch_group_id = 0x05;
      break;
    case 5:	// PresetD
      Part3_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[2].patch_group = 0x00;
      active_area->active_performance.perf_part[2].patch_group_id = 0x06;
      break;
    case 6:	// Exp B
      Part3_PatchNumber_select->setMaximum(256);
      active_area->active_performance.perf_part[2].patch_group = 0x02;
      active_area->active_performance.perf_part[2].patch_group_id = 0x10;
      break;
    case 7:	// Exp C
      Part3_PatchNumber_select->setMaximum(100);
      active_area->active_performance.perf_part[2].patch_group = 0x02;
      active_area->active_performance.perf_part[2].patch_group_id = 0x62;
      break;
    default:
      Part3_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[2].patch_group = 0x00;
      active_area->active_performance.perf_part[2].patch_group_id = 0x00;
      break;
  }	// end switch set Patch group
  // update perf_part.patch_num_high/low
  int pn = Part3_PatchNumber_select->value() - 1;
  active_area->active_performance.perf_part[2].patch_num_high = pn/16;
  active_area->active_performance.perf_part[2].patch_num_low = pn%16;
  if (!state_table->jv_connect) return;
  // update JV  
  unsigned char	buf[8];
  memset(buf,0,sizeof(buf));
  buf[0] = 0x01;
  buf[2] = 0x10 + 0x2;
  buf[3] = 0x02;
  memcpy((void *)&buf[4], (const void *)&active_area->active_performance.perf_part[2].patch_group,4);
  if (sysex_update((const unsigned char*)&buf,8) == EXIT_FAILURE) {
    puts("OOPS 2!"); return;
  }
  Part3_PatchName_display->setText(getPartPatchName(2));
  if (Patch_PerfPartNum_select->currentIndex()==2) {
    state_table->updates_enabled=false;
    Patch_Number_select->setValue(Part3_PatchNumber_select->value());
    Patch_Group_select->setCurrentIndex(Part3_PatchGroup_select->currentIndex());
    Patch_Name_edit->setText(Part3_PatchName_display->text());
    state_table->updates_enabled=true;
  }
}	// end on_Part3_PatchGroup_select_currentIndexChanged

void JVlibForm::on_Part3_PatchNumber_select_valueChanged() {
  on_Part3_PatchGroup_select_currentIndexChanged(Part3_PatchGroup_select->currentIndex());
}	// end on_Part3_PatchNumber_select_valueChanged

void JVlibForm::Part3_SetPatchMax() {
  // set the Maximum possible value for Patch Number based on the current Patch Group
  switch(Part3_PatchGroup_select->currentIndex()) {
    case 0:	// User
      Part3_PatchNumber_select->setMaximum(128);
      break;
    case 1:	// Exp A
      Part3_PatchNumber_select->setMaximum(255);
      break;
    case 2:	// PresetA
      Part3_PatchNumber_select->setMaximum(128);
      break;
    case 3:	// PresetB
      Part3_PatchNumber_select->setMaximum(128);
      break;
    case 4:	// PresetC
      Part3_PatchNumber_select->setMaximum(128);
      break;
    case 5:	// PresetD
      Part3_PatchNumber_select->setMaximum(128);
      break;
    case 6:	// Exp B
      Part3_PatchNumber_select->setMaximum(256);
      break;
    case 7:	// Exp C
      Part3_PatchNumber_select->setMaximum(100);
      break;
    default:
      Part3_PatchNumber_select->setMaximum(128);
      break;
  }
}	// end Part3_SetPatchMax
