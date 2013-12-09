// part_4.cpp
/* Contains:
 * on_Part4_TestTone_switch_toggled
 * on_Part4_ReceiveMidi_enable_toggled
 * on_Part4_MidiChannel_select_valueChanged
 * on_Part4_Level_select_valueChanged
 * on_Part4_Pan_select_valueChanged
 * on_Part4_Transpose_select_valueChanged
 * on_Part4_TuneAdj_select_valueChanged
 * on_Part4_Output_select_currentIndexChanged
 * on_Part4_OutputLevel_select_valueChanged
 * on_Part4_ChorusSend_select_valueChanged
 * on_Part4_ReverbSend_select_valueChanged
 * on_Part4_ReceivePrgChg_enable_toggled
 * on_Part4_ReceiveVolume_enable_toggled
 * on_Part4_ReceiveHold_enable_toggled
 * on_Part4_LowLimit_select_valueChanged
 * on_Part4_HighLimit_select_valueChanged
 * on_Part4_VoiceReserve_select_valueChanged
 * on_Part4_PatchGroup_select_currentIndexChanged
 * Part4_SetPatchMax
 * on_Part4_PatchNumber_select_valueChanged
 */

#include        "JVlibForm.h"
#include        <QtGui>

void JVlibForm::on_Part4_TestTone_switch_toggled(bool val) {
  PartsToneSwitch(Part4_MidiChannel_select->value()-1, val);
  Part4_TestTone_switch->setText(val ? QString::fromUtf8("Stop") : QString::fromUtf8("Play Patch") );
}

//---------------------------------------------------------------------------------------------------------------------
// switches
void JVlibForm::on_Part4_ReceiveMidi_enable_toggled(bool val) {
  if (state_table->perf_mode) setPartSingleValue(3,0,val);
}
void JVlibForm::on_Part4_MidiChannel_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(3,1,val-1);
}
void JVlibForm::on_Part4_Level_select_valueChanged(int val) {
  Part4_Level_select->setStatusTip(QString::number(val));
  if (state_table->perf_mode) setPartSingleValue(3,6,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part4_MidiChannel_select->value()-1,0x07,val);
}
void JVlibForm::on_Part4_Pan_select_valueChanged(int val) {
  Part4_Pan_select->setStatusTip(QString("Pan value: ")+QString::number(val));
  if (state_table->perf_mode) setPartSingleValue(3,7,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part4_MidiChannel_select->value()-1,0x0A,val);
}
void JVlibForm::on_Part4_Transpose_select_valueChanged(int val) { 
  // val is between -48,+48 half-steps
  if (state_table->perf_mode) setPartSingleValue(3, 0x8, val+48);
  // GM wants val between 10h,70h (16->112)
  if (state_table->GM_mode && state_table->updates_enabled) change_12(0xB0+Part4_MidiChannel_select->value()-1,0x65,0x00,0xB0+Part4_MidiChannel_select->value()-1,0x64,0x02,0xB0+Part4_MidiChannel_select->value()-1,0x06,(val+64),0xB0+Part4_MidiChannel_select->value()-1,0x26,0x00);
}
void JVlibForm::on_Part4_TuneAdj_select_valueChanged(int val) { 
  // val is between -50,+50 cents
  if (state_table->perf_mode) setPartSingleValue(3, 0x9, val+50);
  // GM wants val between 20h,60h (32->96)
  if (state_table->GM_mode && state_table->updates_enabled) change_12(0xB0+Part4_MidiChannel_select->value()-1,0x65,0x00,0xB0+Part4_MidiChannel_select->value()-1,0x64,0x01,0xB0+Part4_MidiChannel_select->value()-1,0x06,(val+50)*65/101+32,0xB0+Part4_MidiChannel_select->value()-1,0x26,0x00);
}
void JVlibForm::on_Part4_Output_select_currentIndexChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(3,0xA,val);
}
void JVlibForm::on_Part4_OutputLevel_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(3,0xB,val);
}
void JVlibForm::on_Part4_ChorusSend_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(3,0xC,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part4_MidiChannel_select->value()-1,0x5D,val);
}
void JVlibForm::on_Part4_ReverbSend_select_valueChanged(int val) {
  if (state_table->perf_mode) setPartSingleValue(3,0xD,val);
  if (state_table->GM_mode && state_table->updates_enabled) change_3(0xB0+Part4_MidiChannel_select->value()-1,0x5B,val);
}
void JVlibForm::on_Part4_ReceivePrgChg_enable_toggled(bool val) {
    setPartSingleValue(3,0xE,val);
}
void JVlibForm::on_Part4_ReceiveVolume_enable_toggled(bool val) {
  if (state_table->perf_mode) setPartSingleValue(3,0xF,val);
}
void JVlibForm::on_Part4_ReceiveHold_enable_toggled(bool val) {
  if (state_table->perf_mode) setPartSingleValue(3,0x10,val);
}
void JVlibForm::on_Part4_LowLimit_select_valueChanged(int val) { 
  Part4_LowLimit_display->setText(funcNoteCalc(val));
  if (val>Part4_HighLimit_select->value()) on_Part4_HighLimit_select_valueChanged(val);
  setPartSingleValue(3, 0x11, val);
}
void JVlibForm::on_Part4_HighLimit_select_valueChanged(int val) { 
  Part4_HighLimit_display->setText(funcNoteCalc(val));
  if (val<Part4_LowLimit_select->value()) on_Part4_LowLimit_select_valueChanged(val);
  setPartSingleValue(3, 0x12, val);
}
void JVlibForm::on_Part4_VoiceReserve_select_valueChanged(int val) { 
  setVoiceCounters();
  setPerfSingleValue(0x30+3,val);
}

//---------------------------------------------------------------------------------------------------------------------
void JVlibForm::on_Part4_PatchGroup_select_currentIndexChanged(int val) {
  // called after a change in the Patch group or number for this part to update the Patch Name and active_area memory
  if (!state_table->updates_enabled) return;
  switch(val) {
    case 0:	// User patch
      Part4_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[3].patch_group = 0x0;
      active_area->active_performance.perf_part[3].patch_group_id = 0x01;
      break;
    case 1:	// Exp A
      Part4_PatchNumber_select->setMaximum(255);
      active_area->active_performance.perf_part[3].patch_group = 0x02;
      active_area->active_performance.perf_part[3].patch_group_id = 0x02;
      break;
    case 2:	// PresetA
      Part4_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[3].patch_group = 0x00;
      active_area->active_performance.perf_part[3].patch_group_id = 0x03;
      break;
    case 3:	// PresetB
      Part4_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[3].patch_group = 0x00;
      active_area->active_performance.perf_part[3].patch_group_id = 0x04;
      break;
    case 4:	// PresetC
      Part4_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[3].patch_group = 0x00;
      active_area->active_performance.perf_part[3].patch_group_id = 0x05;
      break;
    case 5:	// PresetD
      Part4_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[3].patch_group = 0x00;
      active_area->active_performance.perf_part[3].patch_group_id = 0x06;
      break;
    case 6:	// Exp B
      Part4_PatchNumber_select->setMaximum(256);
      active_area->active_performance.perf_part[3].patch_group = 0x02;
      active_area->active_performance.perf_part[3].patch_group_id = 0x10;
      break;
    case 7:	// Exp C
      Part4_PatchNumber_select->setMaximum(100);
      active_area->active_performance.perf_part[3].patch_group = 0x02;
      active_area->active_performance.perf_part[3].patch_group_id = 0x62;
      break;
    default:
      Part4_PatchNumber_select->setMaximum(128);
      active_area->active_performance.perf_part[3].patch_group = 0x00;
      active_area->active_performance.perf_part[3].patch_group_id = 0x00;
      break;
  }	// end switch set Patch group
  // update perf_part.patch_num_high/low
  int pn = Part4_PatchNumber_select->value() - 1;
  active_area->active_performance.perf_part[3].patch_num_high = pn/16;
  active_area->active_performance.perf_part[3].patch_num_low = pn%16;
  if (!state_table->jv_connect) return;
  // update JV  
  unsigned char	buf[8];
  memset(buf,0,sizeof(buf));
  buf[0] = 0x01;
  buf[2] = 0x10 + 0x03;
  buf[3] = 0x02;
  memcpy((void *)&buf[4], (const void *)&active_area->active_performance.perf_part[3].patch_group,4);
  if (sysex_update((const unsigned char*)&buf,8) == EXIT_FAILURE) {
    puts("OOPS 2!"); return;
  }
  Part4_PatchName_display->setText(getPartPatchName(3));
}	// end on_Part4_PatchGroup_select_currentIndexChanged

void JVlibForm::on_Part4_PatchNumber_select_valueChanged(int i) {
  on_Part4_PatchGroup_select_currentIndexChanged(Part4_PatchGroup_select->currentIndex());
}	// end on_Part4_PatchNumber_select_valueChanged

void JVlibForm::Part4_SetPatchMax() {
  // set the Maximum possible value for Patch Number based on the current Patch Group
  switch(Part4_PatchGroup_select->currentIndex()) {
    case 0:	// User
      Part4_PatchNumber_select->setMaximum(128);
      break;
    case 1:	// Exp A
      Part4_PatchNumber_select->setMaximum(255);
      break;
    case 2:	// PresetA
      Part4_PatchNumber_select->setMaximum(128);
      break;
    case 3:	// PresetB
      Part4_PatchNumber_select->setMaximum(128);
      break;
    case 4:	// PresetC
      Part4_PatchNumber_select->setMaximum(128);
      break;
    case 5:	// PresetD
      Part4_PatchNumber_select->setMaximum(128);
      break;
    case 6:	// Exp B
      Part4_PatchNumber_select->setMaximum(256);
      break;
    case 7:	// Exp C
      Part4_PatchNumber_select->setMaximum(100);
      break;
    default:
      Part4_PatchNumber_select->setMaximum(128);
      break;
  }
}	// end Part4_SetPatchMax
