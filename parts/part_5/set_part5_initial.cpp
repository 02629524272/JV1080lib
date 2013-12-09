// set_initial.cpp
// for Parts tab, Part_5

#include        "JVlibForm.h"
#include        <QtGui>

void JVlibForm::setPart5_Parms() {
  if (state_table->perf_mode) {
  // set Part 5 controls/displays based on the active area memory contents, 
  // which were just updated from the synth after a Sync button was clicked.
  //  state_table->updates_enabled = false was set by the calling program, do not change it here.
  QString str;
  Part5_Level_select->setValue(active_area->active_performance.perf_part[4].level);
  Part5_Level_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[4].level));
  Part5_Pan_select->setValue(active_area->active_performance.perf_part[4].pan);
  Part5_Pan_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[4].pan));
  Part5_ChorusSend_select->setValue(active_area->active_performance.perf_part[4].chorus_send_level);
  Part5_ChorusSend_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[4].chorus_send_level));
  Part5_ReverbSend_select->setValue(active_area->active_performance.perf_part[4].reverb_send_level);
  Part5_ReverbSend_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[4].reverb_send_level));
  Part5_OutputLevel_select->setValue(active_area->active_performance.perf_part[4].output_level);
  Part5_OutputLevel_select->setStatusTip(str.setNum(active_area->active_performance.perf_part[4].output_level));
  Part5_Transpose_select->setValue(active_area->active_performance.perf_part[4].coarse_tune-48);
  Part5_Transpose_display->display(active_area->active_performance.perf_part[4].coarse_tune-48);
  Part5_TuneAdj_select->setValue(active_area->active_performance.perf_part[4].fine_tune-50);
  Part5_VoiceReserve_select->setValue(active_area->active_performance.perf_common.voice_reserve[4]);
  Part5_LowLimit_select->setValue(active_area->active_performance.perf_part[4].key_lower);
  Part5_HighLimit_select->setValue(active_area->active_performance.perf_part[4].key_upper);
  Part5_ReceiveMidi_enable->setChecked(active_area->active_performance.perf_part[4].MIDI_receive);
  Part5_MidiChannel_select->setValue(active_area->active_performance.perf_part[4].MIDI_channel+1);
  Part5_ReceivePrgChg_enable->setChecked(active_area->active_performance.perf_part[4].receive_program_change);
  Part5_ReceiveVolume_enable->setChecked(active_area->active_performance.perf_part[4].receive_volume);
  Part5_ReceiveHold_enable->setChecked(active_area->active_performance.perf_part[4].receive_hold_1);
  Part5_Output_select->setCurrentIndex(active_area->active_performance.perf_part[4].output);
  Part5_VoiceMode_switch->setChecked(active_area->active_perf_patch[4].patch_common.key_assign_mode);
  Part5_VoiceMode_switch->setText(active_area->active_perf_patch[4].patch_common.key_assign_mode==0 ? QString("Poly") : QString("Solo"));
  
  switch(active_area->active_performance.perf_part[4].patch_group_id) {
    case 0x01:  // User
      Part5_PatchGroup_select->setCurrentIndex(0);
      break;
    case 0x02:  // Exp A
	Part5_PatchGroup_select->setCurrentIndex(1);
	break;
    case 0x03:  // Preset A
	Part5_PatchGroup_select->setCurrentIndex(2);
	break;
    case 0x04:  // Preset B
	Part5_PatchGroup_select->setCurrentIndex(3);
	break;
    case 0x05:  // Preset C
	Part5_PatchGroup_select->setCurrentIndex(4);
	break;
    case 0x06:  // Preset D
	Part5_PatchGroup_select->setCurrentIndex(5);
	break;
    case 0x10:  // Exp B
	Part5_PatchGroup_select->setCurrentIndex(6);
	break;
    case 0x62:  // Exp C
	Part5_PatchGroup_select->setCurrentIndex(7);
	break;
    default:
	Part5_PatchGroup_select->setCurrentIndex(0);
	break;
  }	// end SWITCH
    Part5_PatchNumber_select->setValue((active_area->active_performance.perf_part[4].patch_num_high*16) + active_area->active_performance.perf_part[4].patch_num_low+1);
    Part5_PatchName_display->setText(QString::fromAscii(&active_area->active_perf_patch[4].patch_common.name[0],12));
    Part5_LowLimit_display->setText(funcNoteCalc(Part5_LowLimit_select->value()));
    Part5_HighLimit_display->setText(funcNoteCalc(Part5_HighLimit_select->value()));
//    Part5_PatchGroup_select->setEnabled(Part5_ReceivePrgChg_enable->isChecked() && AcceptBankSel_switch->isChecked()); 
    Part5_MidiChannel_select->setEnabled(Part5_ReceiveMidi_enable->isChecked());
    Part5_ReceivePrgChg_enable->setEnabled(AcceptProgramChg_switch->isChecked() || AcceptBankSel_switch->isChecked());
    Part5_ReceiveVolume_enable->setEnabled(AcceptVolumeChg_switch->isChecked());
    Part5_ReceiveHold_enable->setEnabled(AcceptHold1Chg_switch->isChecked());
  }
  // set GM-MODE only parms
  if (state_table->GM_mode) {
      Part5_PatchGroup_select->setCurrentIndex(5);
      Part5_ReceiveMidi_enable->setChecked(true);
      Part5_ReceivePrgChg_enable->setChecked(true);
      Part5_ReceiveVolume_enable->setChecked(true);
      Part5_ReceiveHold_enable->setChecked(true);
      Part5_PatchNumber_select->setValue(5);
      Part5_PatchName_display->setText(getPartPatchName(4));
      Part5_MidiChannel_select->setValue(5);
      Part5_Transpose_select->setValue(0);
      Part5_TuneAdj_select->setValue(0);
      Part5_ReverbSend_select->setValue(0);
      Part5_ChorusSend_select->setValue(0);
      Part5_Pan_select->setValue(64);
      Part5_Level_select->setValue(127);
      Part5_OutputLevel_select->setValue(127);
      Part5_PatchGroup_select->setEnabled(false);
      Part5_VoiceReserve_select->setEnabled(false);
      Part5_OutputLevel_select->setEnabled(false);
      Part5_Output_select->setEnabled(false);
      Part5_MidiChannel_select->setEnabled(false);
      Part5_ReceivePrgChg_enable->setEnabled(false);
      Part5_ReceiveVolume_enable->setEnabled(false);
      Part5_ReceiveHold_enable->setEnabled(false);
      Part5_ReceiveMidi_enable->setEnabled(false);
      Part5_VoiceMode_switch->setChecked(false);
      Part5_VoiceMode_switch->setText("Poly");
      Part5_VoiceMode_switch->setEnabled(true);
  }
  // following are used for both Perf and GM modes
  Part5_TestTone_switch->setChecked(false);
  Part5_SetPatchMax();
//  Part5_PatchNumber_select->setEnabled(Part5_ReceivePrgChg_enable->isChecked() && AcceptProgramChg_switch->isChecked()); 
  Part5_TestTone_switch->setEnabled(Part5_ReceiveMidi_enable->isChecked());
}	// end setPart5_Parms
