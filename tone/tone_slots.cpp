// tone_slots.cpp
/* Contains:
 * ToneStdUpdate(int offset, int val)
 * setToneSingleValue(int toneNum, int addr, int val)
 * on_ToneSync_button_clicked
 * on_Tone_TestTone_switch_toggled(bool val)
 * on_Tone_WaveChooser_select_currentIndexChanged(int val)
 * on_Tone_InstrFamily_select_currentIndexChanged(int val)
 * on_Tone_Group_select_currentIndexChanged(int val)
 * on_Tone_Number_select_valueChanged(int val)
 * on_Tone_ToneNumber_select_valueChanged()
 * on_Tone_WaveGain_select_currentIndexChanged(int val)
 * on_Tone_FXM_enable_toggled(bool val)
 * on_Tone_FXMColor_select_currentIndexChanged(int val)
 * on_Tone_FXMDepth_select_currentIndexChanged(int val)
 * on_Tone_DelayMode_select_currentIndexChanged(int val)
 * on_Tone_DelayTime_select_valueChanged(int val)
 * on_Tone_VeloXfadeDepth_select_valueChanged(int val)
 * on_Tone_VeloLowerRange_select_valueChanged(int val)
 * on_Tone_VeloUpperRange_select_valueChanged(int val)
 * on_Tone_ReceiveReDamper_enable_toggled(bool val)
 * on_Tone_ReceiveVolume_enable_toggled(bool val)
 * on_Tone_ReceiveHold_enable_toggled(bool val)
 * on_Tone_ReceiveBender_enable_toggled(bool val)
 * on_Tone_C1D1_select_currentIndexChanged(int val)
 * on_Tone_C1D1Depth_select_valueChanged(int val)
 * on_Tone_C1D2_select_currentIndexChanged(int val)
 * on_Tone_C1D2Depth_select_valueChanged(int val)
 * on_Tone_C1D3_select_currentIndexChanged(int val)
 * on_Tone_C1D3Depth_select_valueChanged(int val)
 * on_Tone_C1D4_select_currentIndexChanged(int val)
 * on_Tone_C1D4Depth_select_valueChanged(int val)
 * on_Tone_C2D1_select_currentIndexChanged(int val)
 * on_Tone_C2D1Depth_select_valueChanged(int val)
 * on_Tone_C2D2_select_currentIndexChanged(int val)
 * on_Tone_C2D2Depth_select_valueChanged(int val)
 * on_Tone_C2D3_select_currentIndexChanged(int val)
 * on_Tone_C2D3Depth_select_valueChanged(int val)
 * on_Tone_C2D4_select_currentIndexChanged(int val)
 * on_Tone_C2D4Depth_select_valueChanged(int val)
 * on_Tone_C3D1_select_currentIndexChanged(int val)
 * on_Tone_C3D1Depth_select_valueChanged(int val)
 * on_Tone_C3D2_select_currentIndexChanged(int val)
 * on_Tone_C3D2Depth_select_valueChanged(int val)
 * on_Tone_C3D3_select_currentIndexChanged(int val)
 * on_Tone_C3D3Depth_select_valueChanged(int val)
 * on_Tone_C3D4_select_currentIndexChanged(int val)
 * on_Tone_C3D4Depth_select_valueChanged(int val)
 * on_Tone_Level_select_valueChanged(int val)
 * on_Tone_Output_select_currentIndexChanged(int val)
 * on_Tone_OutputLevel_select_valueChanged(int val)
 * on_Tone_ChorusSend_select_valueChanged(int val)
 * on_Tone_ReverbSend_select_valueChanged(int val)
 */

#include        "JVlibForm.h"
#include        <QtGui>

void JVlibForm::on_ToneSync_button_clicked() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  state_table->updates_enabled = false;
  setToneParms(state_table->perf_mode?Patch_PerfPartNum_select->currentIndex()+1:0);
  Enable_Tone(true);
  state_table->updates_enabled = true;
  QApplication::restoreOverrideCursor();
}	// end on_ToneSync_button_clicked

void JVlibForm::on_Tone_TestTone_switch_toggled(bool val) {
  if (state_table->jv_connect) {
  int pn = state_table->perf_mode ? (Patch_PerfPartNum_select->currentIndex()) : 0;
  if (val) {
    change_3(0x90 + (state_table->perf_mode ? active_area->active_performance.perf_part[pn].MIDI_channel : 
      system_area->sys_common.patch_receive_channel), SysPreviewNote1_select->value(), SysPreviewNote1_volume->value());
  } 
  else {
    change_3(0xB0 + (state_table->perf_mode ? active_area->active_performance.perf_part[pn].MIDI_channel : 
      system_area->sys_common.patch_receive_channel), 0x7B, 0);
  }
  Tone_TestTone_switch->setText(val ? QString::fromUtf8("Stop") : QString::fromUtf8("Play Patch") );
  }
}	// end on_Tone_TestTone_switch_toggled

void JVlibForm::on_Tone_WaveChooser_select_currentIndexChanged(int val) {
  if (val<0) return;	// all entries were removed
  if (!state_table->updates_enabled) return;
  QString group = Tone_WaveChooser_select->currentText().section(" ",-3,-2);
  int x = Tone_Group_select->findText(group);
  if (Tone_Group_select->currentIndex() != x) {
    Tone_Group_select->blockSignals(true);
    Tone_Group_select->setCurrentIndex(x);
    Tone_Group_select->blockSignals(false);
  }
  x = Tone_WaveChooser_select->currentText().section(" ",-1).toInt();
  if (Tone_Number_select->value() != x) {
    Tone_Number_select->blockSignals(true);
    Tone_Number_select->setValue(x);
    Tone_Number_select->blockSignals(false);
  }
  // update Tone_Name with the new value
  Tone_WaveName_display->setText(WaveName_query());
  ToneEFX_WaveName_display->setText(Tone_WaveName_display->text());
  ToneTVF_WaveName_display->setText(Tone_WaveName_display->text());
  ToneTVA_WaveName_display->setText(Tone_WaveName_display->text());
  Pitch_WaveName_display->setText(Tone_WaveName_display->text());
  // now update the synth
  if (!state_table->jv_connect) return;
  int tn = Tone_ToneNumber_select->value() - 1;
  x = Tone_Number_select->value() - 1;
  int Hval = x/16;
  int Lval = x%16;
  unsigned char buf[8];
  buf[0] = state_table->perf_mode?0x02:0x03;	// are we in Perf or Patch mode?
  buf[1] = 0x00 + state_table->perf_mode ? Patch_PerfPartNum_select->currentIndex() : 0;	// select the Perf Part, if in that mode
  buf[2] = 0x10 + (tn*2);
  buf[3] = 0x01;
  buf[4] = (Tone_Group_select->currentIndex()<2?0:2);	// wave_group
  // wave_group_id
  switch(Tone_Group_select->currentIndex()) {
	case 0:
	default:
	  buf[5] = 0x01;
	  break;
	case 1:
	case 2:
	  buf[5] = 0x02;
	  break; 
	case 3:
	  buf[5] = 0x10;
	  break; 
	case 4:
	  buf[5] = 0x62;
	  break;
  }	// end Switch
  buf[6] = Hval;
  buf[7] = Lval;
//  if (open_ports() == EXIT_FAILURE) return;
  if (sysex_update(&buf[0],8) == EXIT_FAILURE) {
//    close_ports(); 
    return;
  }
//  close_ports();
  state_table->tone_modified = true;
}	// end on_Tone_WaveChooser_select_currentIndexChanged

void JVlibForm::on_Tone_InstrFamily_select_currentIndexChanged(int val) {
  // fill Tone_WaveChooser_select with valid waves from the database
  if (val<0) return;	// cleared all entries, called from setToneParms
  QSqlQuery query(mysql);
  query.prepare("Select name,group_area,number from wave_list where Instruments = ? order by name, group_area, number");
  query.bindValue(0, Tone_InstrFamily_select->currentText());
  if (query.exec() == false) {
    puts("Query exec failed in on_Tone_InstrFamily_select_currentIndexChanged");
    QMessageBox::critical(this, "JVlib", QString("Query failed in setWaveChooser for query\n%1") .arg(query.executedQuery()));
    query.finish();
    return;
  }
  if (query.size()==0) {
    puts("0 rows found in on_Tone_InstrFamily_select_currentIndexChanged"); 
    QMessageBox::critical(this, "JVlib", QString("0 rows returned in setWaveChooser for query\n%1") .arg(query.executedQuery()));
    query.finish();
    return;
  }
  Tone_WaveChooser_select->blockSignals(true);
  Tone_WaveChooser_select->clear();
  while (query.next()) {
    Tone_WaveChooser_select->insertItem(0,query.value(0).toString()+QString(" ")+query.value(1).toString()+QString(" ")+query.value(2).toString());
  }
  query.finish();
  Tone_WaveChooser_select->blockSignals(false);
//  Tone_WaveChooser_select->setCurrentIndex(0);
}	// end on_Tone_InstrFamily_select_currentIndexChanged

void JVlibForm::on_Tone_Group_select_currentIndexChanged(int val) {
  if (state_table->updates_enabled) {
    int tn = Tone_ToneNumber_select->value() - 1;
    int pn = 0;
    // update local memory
    if (state_table->perf_mode) {
      int pn = Patch_PerfPartNum_select->currentIndex();
      switch(val) {
	case 0:
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group = 0x00;
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group_id = 0x01;
	  break;
	case 1:
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group = 0x00;
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group_id = 0x02;
	  break;
	case 2:
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group = 0x02;
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group_id = 0x02;
	  break;
	case 3:
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group = 0x02;
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group_id = 0x10;
	  break;
	case 4:
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group = 0x02;
	  active_area->active_perf_patch[pn].patch_tone[tn].wave_group_id = 0x62;
	  break;
      }
    } else {
      switch(val) {
	case 0:
	  active_area->active_patch_patch.patch_tone[tn].wave_group = 0x00;
	  active_area->active_patch_patch.patch_tone[tn].wave_group_id = 0x01;
	  break;
	case 1:
	  active_area->active_patch_patch.patch_tone[tn].wave_group = 0x00;
	  active_area->active_patch_patch.patch_tone[tn].wave_group_id = 0x02;
	  break;
	case 2:
	  active_area->active_patch_patch.patch_tone[tn].wave_group = 0x02;
	  active_area->active_patch_patch.patch_tone[tn].wave_group_id = 0x02;
	  break;
	case 3:
	  active_area->active_patch_patch.patch_tone[tn].wave_group = 0x02;
	  active_area->active_patch_patch.patch_tone[tn].wave_group_id = 0x10;
	  break;
	case 4:
	  active_area->active_patch_patch.patch_tone[tn].wave_group = 0x02;
	  active_area->active_patch_patch.patch_tone[tn].wave_group_id = 0x62;
	  break;
      }
    }
    if (state_table->jv_connect) {
    // update the synth
      unsigned char buf[6];
      memset(buf,0,sizeof(buf));
      buf[0] = state_table->perf_mode?0x02:0x03;	// are we in Perf or Patch mode?
      buf[1] = 0x00 + pn;	// select the Perf Part, if in that mode
      buf[2] = 0x10 + (tn*2);
      buf[3] = 0x01;
      buf[4] = (val<2?0:2);	// wave_group
      // wave_group_id
      buf[5] = state_table->perf_mode ? active_area->active_perf_patch[pn].patch_tone[tn].wave_group_id : active_area->active_patch_patch.patch_tone[tn].wave_group_id;
//      if (open_ports() == EXIT_FAILURE) {
//	puts("Error! Unable to open MIDI ports in on_Tone_Group_select_currentIndexChanged");
//	return;
//      }
      if (sysex_update(&buf[0],6) == EXIT_FAILURE) {
	puts("Error! Unable to send sysex data in on_Tone_Group_select_currentIndexChanged");
//	close_ports(); 
	return;
      }
//      close_ports();
    }	// end state_table->jv_connect
    // set Number maximum
    QSqlQuery query(mysql);
    query.prepare("Select number from wave_list where group_area = ?");
    query.bindValue(0, Tone_Group_select->currentText());
    if (query.exec() == false) {
      puts("Query exec failed in on_Tone_Group_select_currentIndexChanged");
      QMessageBox::critical(this, "JVlib", QString("Query failed in setWaveChooser for query\n%1") .arg(query.executedQuery()));
      query.finish();
      return;
    }
    if (query.size()==0) {
      puts("0 rows found in on_Tone_Group_select_currentIndexChanged"); 
      QMessageBox::critical(this, "JVlib", QString("0 rows returned in setWaveChooser for query\n%1") .arg(query.executedQuery()));
      query.finish();
      return;
    }
    Tone_Number_select->setMaximum((int)query.size());
    query.finish();
    setWaveChooser();
  }	// end state_table->updates_enabled
}	// end on_Tone_Group_select_currentIndexChanged

void JVlibForm::on_Tone_Number_select_valueChanged(int val) {
  if (state_table->updates_enabled) {
    int tn = Tone_ToneNumber_select->value() - 1;
    int pn = state_table->perf_mode ? Patch_PerfPartNum_select->currentIndex() : 0;
    int Hval = (val-1)/16;
    int Lval = (val-1)%16;
    // update local memory
    if (state_table->perf_mode) {
      active_area->active_perf_patch[pn].patch_tone[tn].wave_num_high = Hval;
      active_area->active_perf_patch[pn].patch_tone[tn].wave_num_low = Lval;
    } else {
      active_area->active_patch_patch.patch_tone[tn].wave_num_high = Hval;
      active_area->active_patch_patch.patch_tone[tn].wave_num_low = Lval;
    }
    if (state_table->jv_connect) {
    // update the synth
      unsigned char buf[6];
      buf[0] = state_table->perf_mode?0x02:0x03;	// are we in Perf or Patch mode?
      buf[1] = 0x00 + state_table->perf_mode ? pn : 0;	// select the Perf Part, if in that mode
      buf[2] = 0x10 + (tn*2);
      buf[3] = 0x03;
      buf[4] = Hval;
      buf[5] = Lval;
//      if (open_ports() == EXIT_FAILURE) return;
      if (sysex_update(&buf[0],6) == EXIT_FAILURE) {
//	close_ports(); 
	return;
      }
//      close_ports();
      state_table->tone_modified = true;
    }	// end state_table->jv_connect
   setWaveChooser();
   ToneEFX_ToneNumber_display->setText(Tone_ToneNumber_select->cleanText());
   ToneTVF_ToneNumber_display->setText(Tone_ToneNumber_select->cleanText());
   ToneTVA_ToneNumber_display->setText(Tone_ToneNumber_select->cleanText());
   Pitch_ToneNumber_display->setText(Tone_ToneNumber_select->cleanText());
  }	// end UPDATES_enabled
}	// end on_Tone_Number_select_valueChanged

void JVlibForm::ToneStdUpdate(int offset, int val) {
  if (state_table->updates_enabled) {
    char *ptr;
    int tn = Tone_ToneNumber_select->value() - 1;
    if (state_table->perf_mode)
      ptr = &active_area->active_perf_patch[Patch_PerfPartNum_select->currentIndex()].patch_tone[tn].wave_group;
    else
      ptr = &active_area->active_patch_patch.patch_tone[tn].wave_group;
    ptr[offset-1] = val;
    if (state_table->jv_connect) 
      setToneSingleValue(tn,offset, val);
  }	// end state_table->updates_enabled
}	// end ToneStdUpdate

void JVlibForm::setToneSingleValue(int toneNum, int addr, int val) {
  // automatically determines if we are in Performance or Patch mode, and sets the correct patch tone value as needed
  if (state_table->updates_enabled && state_table->jv_connect) {
    unsigned char buf[6];
    buf[0] = state_table->perf_mode?0x02:0x03;	// are we in Perf or Patch mode?
    buf[1] = 0x00 + (state_table->perf_mode ? Patch_PerfPartNum_select->currentIndex() : 0);	// select the Perf Part, if in that mode
    buf[2] = 0x10 + (toneNum*2) + (addr>0x7F ? 1 : 0);
    buf[3] = addr>0x7F ? addr-0x80 : addr;
    buf[4] = val;
//    if (open_ports() == EXIT_FAILURE) return;
    if (sysex_update(&buf[0],5) == EXIT_FAILURE) {
//      close_ports(); 
      return;
    }
//    close_ports();
    state_table->tone_modified = true;
  }
}	// end setToneSingleValue
//---------------------------------------------------------------------------------------------------------------------
// switches
void JVlibForm::on_Tone_ToneNumber_select_valueChanged() {
  state_table->updates_enabled = false;
  setToneParms(state_table->perf_mode?Patch_PerfPartNum_select->currentIndex()+1:0);
  state_table->updates_enabled = true;
}	// end on_Tone_ToneNumber_select_valueChanged

void JVlibForm::on_Tone_WaveGain_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x05, val);
}
void JVlibForm::on_Tone_FXM_enable_toggled(bool val) {
  ToneStdUpdate(0x06, val);
}
void JVlibForm::on_Tone_FXMColor_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x07, val);  
}
void JVlibForm::on_Tone_FXMDepth_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x08, val);
}
void JVlibForm::on_Tone_DelayMode_select_currentIndexChanged(int val) {
  if (state_table->updates_enabled) {
    state_table->updates_enabled=false;
    on_Tone_DelayTime_select_valueChanged(Tone_DelayTime_select->value());
    state_table->updates_enabled=true;
  }
  ToneStdUpdate(0x09, val);
}
void JVlibForm::on_Tone_DelayTime_select_valueChanged(int val) {
  QString buf;
  if (Tone_DelayMode_select->currentIndex()==3 || Tone_DelayMode_select->currentIndex()==4) {
    if (val<49) {
      buf = QString::number(val*2);
      if (val<6) buf += " <32nd";
      else if (val==6) buf += " =32nd";
      else if (val<8) buf += " <16th Triplet";
      else if (val==8) buf += " =16th Triplet";
      else if (val==9) buf += " =dotted 32nd";
      else if (val<12) buf += " <16th";
      else if (val==12) buf += " =16th";
      else if (val<16) buf += " <8th Triplet";
      else if (val==16) buf += " =8th Triplet";
      else if (val<18) buf += " <dotted 16th";
      else if (val==18) buf += " =dotted 16th";
      else if (val<24) buf += " <8th";
      else if (val==24) buf += " =8th";
      else if (val<32) buf += " <Quarter Trp";
      else if (val==32) buf += " =Quarter Trp";
      else if (val<36) buf += " <dotted 8th";
      else if (val==36) buf += " =dotted 8th";
      else if (val<48) buf += " <Quarter";
      else  buf += " =Quarter";
    } else if (val<73) {
      buf = QString::number((val-24)*4);
      if (val<56) buf += " <Half Tpl";
      else if (val==56) buf += " =Half Tpl";
      else if (val<60) buf += " <dotted Quarter";
      else if (val==60) buf += " =dotted Quarter";
      else if (val<72) buf += " <Halfnote";
      else  buf += " =Halfnote";
    } else if (val<97) {
      buf = QString::number((val-48)*8);
      if (val<80)  buf += " <Wholenote Trp";
      else if (val==80) buf += " =Wholenote Trp";
      else if (val<84) buf += " <dotted Half";
      else if (val==84) buf += " =dotted Half";
      else if (val<96) buf += " <Wholenote";
      else buf += " =Wholenote";
    } else {
      buf = QString::number((val-72)*16);
      if (val<104) buf += " <DlbWhole Trp";
      else if (val==104) buf += " =DlbWhole Trp";
      else if (val<108) buf += " <dotted Wholenote";
      else if (val==108) buf += " =dotted Wholenote";
      else if (val<120) buf += " <DblWhole";
      else if (val==120) buf += " =DblWhole";
      else  buf += " >DblWhole";
    }
  } else {
    buf = QString::number(val);
  }
  Tone_DelayTime_display->setText(buf);
  ToneStdUpdate(0x0A, val);
}	// end on_Tone_DelayTime_select_valueChanged

void JVlibForm::on_Tone_VeloXfadeDepth_select_valueChanged(int val) {
  ToneStdUpdate(0x0B, val);
}
void JVlibForm::on_Tone_VeloLowerRange_select_valueChanged(int val) {
  if (val>Tone_VeloUpperRange_select->value())
    Tone_VeloUpperRange_select->setValue(val);
  ToneStdUpdate(0x0C, val);
}
void JVlibForm::on_Tone_VeloUpperRange_select_valueChanged(int val) {
  if (val<Tone_VeloLowerRange_select->value())
    Tone_VeloLowerRange_select->setValue(val);
  ToneStdUpdate(0x0D, val);
}
void JVlibForm::on_Tone_ReceiveReDamper_enable_toggled(bool val) {
  ToneStdUpdate(0x10, val);
}
void JVlibForm::on_Tone_ReceiveVolume_enable_toggled(bool val) {
  ToneStdUpdate(0x11, val);
}
void JVlibForm::on_Tone_ReceiveHold_enable_toggled(bool val) {
  ToneStdUpdate(0x12, val);
}
void JVlibForm::on_Tone_ReceiveBender_enable_toggled(bool val) {
  ToneStdUpdate(0x13, val);
}
void JVlibForm::on_Tone_C1D1_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x15, val);
}
void JVlibForm::on_Tone_C1D1Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x16, val+63);
}
void JVlibForm::on_Tone_C1D2_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x17, val);
}
void JVlibForm::on_Tone_C1D2Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x18, val+63);
}
void JVlibForm::on_Tone_C1D3_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x19, val);
}
void JVlibForm::on_Tone_C1D3Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x1A, val+63);
}
void JVlibForm::on_Tone_C1D4_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x1B, val);
}
void JVlibForm::on_Tone_C1D4Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x1C, val+63);
}
void JVlibForm::on_Tone_C2D1_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x1D, val);
}
void JVlibForm::on_Tone_C2D1Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x1E, val+63);
}
void JVlibForm::on_Tone_C2D2_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x1F, val);
}
void JVlibForm::on_Tone_C2D2Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x20, val+63);
}
void JVlibForm::on_Tone_C2D3_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x21, val);
}
void JVlibForm::on_Tone_C2D3Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x22, val+63);
}
void JVlibForm::on_Tone_C2D4_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x23, val);
}
void JVlibForm::on_Tone_C2D4Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x24, val+63);
}
void JVlibForm::on_Tone_C3D1_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x25, val);
}
void JVlibForm::on_Tone_C3D1Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x26, val+63);
}
void JVlibForm::on_Tone_C3D2_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x27, val);
}
void JVlibForm::on_Tone_C3D2Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x28, val+63);
}
void JVlibForm::on_Tone_C3D3_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x29, val);
}
void JVlibForm::on_Tone_C3D3Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x2A, val+63);
}
void JVlibForm::on_Tone_C3D4_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x2B, val);
}
void JVlibForm::on_Tone_C3D4Depth_select_valueChanged(int val) {
  ToneStdUpdate(0x2C, val+63);
}
void JVlibForm::on_Tone_Level_select_valueChanged(int val) {
  ToneStdUpdate(0x65, val);
}
void JVlibForm::on_Tone_Output_select_currentIndexChanged(int val) {
  ToneStdUpdate(0x7D, val);
}
void JVlibForm::on_Tone_OutputLevel_select_valueChanged(int val) {
  ToneStdUpdate(0x7E, val);
}
void JVlibForm::on_Tone_ChorusSend_select_valueChanged(int val) {
  ToneStdUpdate(0x7F, val);
}
void JVlibForm::on_Tone_ReverbSend_select_valueChanged(int val) {
  ToneStdUpdate(0x80, val);
}
