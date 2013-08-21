// set_initial.cpp
// for Parts tab, Part_2

#include        "JVlibForm.h"
#include        <QtGui>

void JVlibForm::setPart2_Parms() {
  if (state_table->perf_mode) {
  // set Part 2 controls/displays based on the active area memory contents, 
  // which were just updated from the synth after a Sync button was clicked.
  //  state_table->updates_enabled = false was set by the calling program, do not change it here.
  QString str;
  Part2_Level_select->setValue(active_area->active_performance.perf_part[1].level);
  Part2_Level_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[1].level));
  Part2_Pan_select->setValue(active_area->active_performance.perf_part[1].pan);
  Part2_Pan_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[1].pan));
  Part2_ChorusSend_select->setValue(active_area->active_performance.perf_part[1].chorus_send_level);
  Part2_ChorusSend_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[1].chorus_send_level));
  Part2_ReverbSend_select->setValue(active_area->active_performance.perf_part[1].reverb_send_level);
  Part2_ReverbSend_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[1].reverb_send_level));
  Part2_OutputLevel_select->setValue(active_area->active_performance.perf_part[1].output_level);
  Part2_OutputLevel_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[1].output_level));
  Part2_Transpose_select->setValue(active_area->active_performance.perf_part[1].coarse_tune-48);
  Part2_Transpose_display->display(active_area->active_performance.perf_part[1].coarse_tune-48);
  Part2_TuneAdj_select->setValue(active_area->active_performance.perf_part[1].fine_tune-50);
  Part2_VoiceReserve_select->setValue(active_area->active_performance.perf_common.voice_reserve[1]);
  Part2_LowLimit_select->setValue(active_area->active_performance.perf_part[1].key_lower);
  Part2_HighLimit_select->setValue(active_area->active_performance.perf_part[1].key_upper);
  Part2_ReceiveMidi_enable->setChecked(active_area->active_performance.perf_part[1].MIDI_receive);
  Part2_MidiChannel_select->setValue(active_area->active_performance.perf_part[1].MIDI_channel+1);
  Part2_ReceivePrgChg_enable->setChecked(active_area->active_performance.perf_part[1].receive_program_change);
  Part2_ReceiveVolume_enable->setChecked(active_area->active_performance.perf_part[1].receive_volume);
  Part2_ReceiveHold_enable->setChecked(active_area->active_performance.perf_part[1].receive_hold_1);
  Part2_Output_select->setCurrentIndex(active_area->active_performance.perf_part[1].output);
  Part2_VoiceMode_switch->setChecked(active_area->active_perf_patch[1].patch_common.key_assign_mode);
  Part2_VoiceMode_switch->setText(active_area->active_perf_patch[1].patch_common.key_assign_mode==0 ? QString("Poly") : QString("Solo"));
  
  switch(active_area->active_performance.perf_part[1].patch_group_id) {
    case 0x01:  // User
      Part2_PatchGroup_select->setCurrentIndex(0);
      break;
    case 0x02:  // Exp A
	Part2_PatchGroup_select->setCurrentIndex(1);
	break;
    case 0x03:  // Preset A
	Part2_PatchGroup_select->setCurrentIndex(2);
	break;
    case 0x04:  // Preset B
	Part2_PatchGroup_select->setCurrentIndex(3);
	break;
    case 0x05:  // Preset C
	Part2_PatchGroup_select->setCurrentIndex(4);
	break;
    case 0x06:  // Preset D
	Part2_PatchGroup_select->setCurrentIndex(5);
	break;
    case 0x10:  // Exp B
	Part2_PatchGroup_select->setCurrentIndex(6);
	break;
    case 0x62:  // Exp C
	Part2_PatchGroup_select->setCurrentIndex(7);
	break;
    default:
	Part2_PatchGroup_select->setCurrentIndex(0);
	break;
  }	// end SWITCH
  Part2_LowLimit_display->setText(funcNoteCalc(Part2_LowLimit_select->value()));
  Part2_HighLimit_display->setText(funcNoteCalc(Part2_HighLimit_select->value()));
  Part2_PatchNumber_select->setValue((active_area->active_performance.perf_part[1].patch_num_high*16) + active_area->active_performance.perf_part[1].patch_num_low+1);
  Part2_PatchName_display->setText(QString::fromAscii(&active_area->active_perf_patch[1].patch_common.name[0],12));
  Part2_PatchGroup_select->setEnabled(Part2_ReceivePrgChg_enable->isChecked()); 
  Part2_MidiChannel_select->setEnabled(Part2_ReceiveMidi_enable->isChecked());
  Part2_ReceivePrgChg_enable->setEnabled(AcceptProgramChg_switch->isChecked());
  Part2_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
  Part2_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  }
  // set GM-MODE only parms
  if (state_table->GM_mode) {
      Part2_PatchGroup_select->setCurrentIndex(5);
      Part2_ReceiveMidi_enable->setChecked(true);
      Part2_ReceivePrgChg_enable->setChecked(true);
      Part2_ReceiveVolume_enable->setChecked(true);
      Part2_ReceiveHold_enable->setChecked(true);
      Part2_PatchNumber_select->setValue(2);
      Part2_PatchName_display->setText(getPartPatchName(1));
      Part2_MidiChannel_select->setValue(2);
      Part2_Transpose_select->setValue(0);
      Part2_TuneAdj_select->setValue(0);
      Part2_ReverbSend_select->setValue(0);
      Part2_ChorusSend_select->setValue(0);
      Part2_Pan_select->setValue(64);
      Part2_Level_select->setValue(127);
      Part2_OutputLevel_select->setValue(127);
      Part2_PatchGroup_select->setEnabled(false);
      Part2_VoiceReserve_select->setEnabled(false);
      Part2_OutputLevel_select->setEnabled(false);
      Part2_Output_select->setEnabled(false);
      Part2_MidiChannel_select->setEnabled(false);
      Part2_ReceivePrgChg_enable->setEnabled(false);
      Part2_ReceiveVolume_enable->setEnabled(false);
      Part2_ReceiveHold_enable->setEnabled(false);
      Part2_ReceiveMidi_enable->setEnabled(false);
      Part2_VoiceMode_switch->setChecked(false);
      Part2_VoiceMode_switch->setText("Poly");
      Part2_VoiceMode_switch->setEnabled(true);
  }
  // following are used for both Perf and GM modes
  Part2_TestTone_switch->setChecked(false);
  Part2_SetPatchMax();
  Part2_PatchNumber_select->setEnabled(Part2_ReceivePrgChg_enable->isChecked()); 
  Part2_TestTone_switch->setEnabled(Part2_ReceiveMidi_enable->isChecked());
  
}	// end setPart2_Parms
