// patch.cpp
/* includes
 * EnablePatch()
 * getSinglePerfPatch()
 * getActivePatchMode()
 * setPatchSingleValue()
 * Patch_setPatchMax()
 */

#include	"JVlibForm.h"
#include	<QtGui>

void JVlibForm::EnablePatch(bool val) {
  // called after Patch values are downloaded from the synth
  // also called to disable/enable controls when the port is changed, until the Sync button is pressed again.

  Patch_PerfPartNum_select->setEnabled(state_table->perf_mode ? true : false);
  Patch_PerfPartNum_select->setToolTip(state_table->perf_mode ? "Select the Part Number to view" : "");
  Pitch_OctaveShift_select->setEnabled(val);
  Patch_HoldPeak_select->setEnabled(val);
  Patch_C1_HoldPeak_select->setEnabled(val);
  Patch_C2_HoldPeak_select->setEnabled(val);
  Patch_C3_HoldPeak_select->setEnabled(val);
  Patch_VelocityRange_enable->setEnabled(val);
  Patch_ControlSource2_select->setEnabled(val);
  Patch_ControlSource3_select->setEnabled(val);
  Patch_BenderDown_select->setEnabled(val);
  Patch_BenderUp_select->setEnabled(val);
  Pitch_StretchTuning_select->setEnabled(val);
  Patch_Struct12_box->setEnabled(val);
  Patch_STruct34_box->setEnabled(val);
  Patch_TempoBox->setEnabled(state_table->perf_mode ? false : val);
  Patch_DefaultTempo_select->setToolTip(state_table->patch_mode ? "Set the default tempo for the Patch" : "Performance tempo controls all patches");
  Patch_Tone1_enable->setEnabled(val);
  Patch_Tone2_enable->setEnabled(val);
  Patch_Tone3_enable->setEnabled(val);
  Patch_Tone4_enable->setEnabled(val);
  Patch_Level_select->setEnabled(val);
  Patch_Pan_select->setEnabled(val);
  Patch_AnalogFeel_select->setEnabled(val);
  Patch_PortamentoTime_select->setEnabled(val);
  Patch_VoicePriority_switch->setEnabled(val);
  Patch_VoiceMode_switch->setEnabled(val);
  Patch_Portamento_box->setEnabled(val);
  Patch_TestTone_switch->setEnabled(val);
  Patch_TestTone_switch->setChecked(false);
  PatchEFX_EFX_groupBox->setEnabled(val);
  PatchEFX_Reverb_box->setEnabled(val);
  PatchEFX_Chorus_box->setEnabled(val);
  PatchEFX_TestTone_switch->setEnabled(val);
  PatchEFX_TestTone_switch->setChecked(false);
  ToneSync_button->setEnabled(val);
  setPatchTabs(val);
  if (val) {
    Patch_Sync_status->on();
    Tone_Sync_status->on();
  } else {
    Patch_Sync_status->off();
    Tone_Sync_status->off();
  }
  state_table->patch_sync = val;
}	// end EnablePatch

void JVlibForm::getSinglePerfPatch(int pn) {
  // Called when Patch sync button is pressed in System Performance mode, or manually for Save_Dialog
  // Parm passed is the Patch_PerfPartNum_select->currentIndex() or other desired Perf Patch,
  // which is one less than the actual Part number
  // Download the patch common and up to 4 tones per Patch

//  int pn = Patch_PerfPartNum_select->currentIndex();
  switch(pn) {
    case 0:
      if (state_table->part1_sync) return;
      break;
    case 1:
      if (state_table->part2_sync) return;
      break;
    case 2:
      if (state_table->part3_sync) return;
      break;
    case 3:
      if (state_table->part4_sync) return;
      break;
    case 4:
      if (state_table->part5_sync) return;
      break;
    case 5:
      if (state_table->part6_sync) return;
      break;
    case 6:
      if (state_table->part7_sync) return;
      break;
    case 7:
      if (state_table->part8_sync) return;
      break;
    case 8:
      if (state_table->part9_sync) return;
      break;
    case 9:
      return;
      break;
    case 10:
      if (state_table->part11_sync) return;
      break;
    case 11:
      if (state_table->part12_sync) return;
      break;
    case 12:
      if (state_table->part13_sync) return;
      break;
    case 13:
      if (state_table->part14_sync) return;
      break;
    case 14:
      if (state_table->part15_sync) return;
      break;
    case 15:
      if (state_table->part16_sync) return;
      break;
  }	// end switch
  
  int   err;
  int   Stop=0;
  unsigned char  buf[8];
  char    patch_common_size[] = { 0x0,0x0,0x00,0x48 };
  char    patch_tone_size[] = { 0x0,0x0,0x01,0x01 };
  memset(buf,0,sizeof(buf));
  buf[0] = 0x02;
  buf[1] = pn;		// Patch number
  buf[2] = 0;
  buf[7] = 0x48;
  usleep(200000);
  if (open_ports() == EXIT_FAILURE) return;
  RetryC:
  usleep(200000*Stop);
  if (sysex_request(buf,8) == EXIT_FAILURE) { close_ports(); return; }
  err = sysex_get((unsigned char *)&active_area->active_perf_patch[pn].patch_common.name[0], (char *)patch_common_size);
  if (err == EXIT_FAILURE) { close_ports(); puts("error1"); return; }
  if (err==2 && Stop<MAX_RETRIES) { Stop++; goto RetryC; }
  if (err==3 && Stop<MAX_RETRIES) { Stop++; goto RetryC; }
  if (err != EXIT_SUCCESS) { close_ports(); puts("error2"); return; }
  Stop=0;
  // build parms for 4 tones
  memcpy(buf+4,patch_tone_size,4);
  for (int y=0;y<4;y++) {		// get 4 tones
    buf[2] = 0x10+(y*2);	// tone address
    usleep(200000);
    RetryD:
    usleep(200000*Stop);
    if (sysex_request(buf,8) == EXIT_FAILURE) { close_ports(); return; }
    err = sysex_get((unsigned char *)&active_area->active_perf_patch[pn].patch_tone[y].tone, (char *)patch_tone_size);
    if (err == EXIT_FAILURE) { close_ports(); puts("error3"); return; }
    if (err==2 && Stop<MAX_RETRIES) { Stop++; goto RetryD; }
    if (err==3 && Stop<MAX_RETRIES) { Stop++; goto RetryD; }
    if (err != EXIT_SUCCESS) { close_ports(); puts("error4"); return; }
    Stop=0;
  }	// end FOR 4 tones
  close_ports();
  switch(pn) {
    case 0:
      state_table->part1_sync = true;
      break;
    case 1:
      state_table->part2_sync = true;
      break;
    case 2:
      state_table->part3_sync = true;
      break;
    case 3:
      state_table->part4_sync = true;
      break;
    case 4:
      state_table->part5_sync = true;
      break;
    case 5:
      state_table->part6_sync = true;
      break;
    case 6:
      state_table->part7_sync = true;
      break;
    case 7:
      state_table->part8_sync = true;
      break;
    case 8:
      state_table->part9_sync = true;
      break;
    case 9:
      break;
    case 10:
      state_table->part11_sync = true;
      break;
    case 11:
      state_table->part12_sync = true;
      break;
    case 12:
      state_table->part13_sync = true;
      break;
    case 13:
      state_table->part14_sync = true;
      break;
    case 14:
      state_table->part15_sync = true;
      break;
    case 15:
      state_table->part16_sync = true;
      break;
  }	// end switch
 state_table->patch_modified = false;
 state_table->patch_sync = true;
}	// end getSinglePerfPatch

void JVlibForm::getActivePatchMode() {
  // called when Patch Sync button is clicked and we are in System Patch mode
  // get active_area patch mode patch, common and 4 tones
  int   err;
  int   Stop=0;
  unsigned char  buf[8];
  char active_patch_patch_common[]={ 0x3,0x0,0x0,0x0 };        // 2rd byte is patch number
  char    patch_tone_size[] = { 0x0,0x0,0x01,0x01 };
  char    patch_common_size[] = { 0x0,0x0,0x00,0x48 };
  
  memset(buf,0,sizeof(buf));
  memcpy(buf,active_patch_patch_common,4);
  memcpy(buf+4,patch_common_size,4);
  if (open_ports() == EXIT_FAILURE) return;
  QProgressDialog progress("Getting patch data...", "Abort Download", 0, 4, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(0);
  RetryE:
  progress.setValue(0);
  if (sysex_request(buf,8) == EXIT_FAILURE) { close_ports(); return; }
  err = sysex_get((unsigned char *)&active_area->active_patch_patch.patch_common.name[0], (char *)patch_common_size);
  if (err == EXIT_FAILURE) { close_ports(); return; }
  if (err==2 && Stop<MAX_RETRIES) {  Stop++; usleep(20000*Stop); goto RetryE; }
  if (err==3 && Stop<MAX_RETRIES) {  Stop++; usleep(20000*Stop); goto RetryE; }
  if (err != EXIT_SUCCESS) { close_ports(); return; }
  Stop=0;
  puts("Patch common downloaded");
  // build parms for 4 tones
  memcpy(buf+4,patch_tone_size,4);
  for (int y=0;y<4;y++) {		// get 4 tones
    progress.setValue(y);
    if (progress.wasCanceled()) return;
    buf[2] = 0x10+(y*2);	// tone address
    RetryF:
    if (sysex_request(buf,8) == EXIT_FAILURE) { close_ports(); return; }
    err = sysex_get((unsigned char *)&active_area->active_patch_patch.patch_tone[y].tone, (char *)patch_tone_size);
    if (err == EXIT_FAILURE) { close_ports(); puts("error3"); return; }
    if (err==2 && Stop<MAX_RETRIES) { Stop++; usleep(20000*Stop); goto RetryF; }
    if (err==3 && Stop<MAX_RETRIES) { Stop++; usleep(20000*Stop); goto RetryF; }
    if (err != EXIT_SUCCESS) { close_ports(); puts("error4"); return; }
    Stop=0;
  printf("Patch mode Tone %d downloaded\n", y+1);
  }	// end FOR 4 tones
  close_ports();
  progress.setValue(4);
  progress.reset();
  state_table->patch_modified = false;
  state_table->patch_sync = true;
  statusbar->showMessage("Active Patch Loaded");
//  setToneParms(0);
  Enable_Tone(true);
}	// end getActivePatcheModePatch

void JVlibForm::setPatchSingleValue(int addr, int val) {
  // automatically determines if we are in Performance or Patch mode, and sets the correct patch value as needed
  if (state_table->jv_connect && state_table->updates_enabled) {
    unsigned char buf[5];
    buf[0] = (state_table->perf_mode?0x02:0x03);	// are we in Perf or Patch mode?
    buf[1] = 0x00 + (state_table->perf_mode?Patch_PerfPartNum_select->currentIndex() : 0);	// select the Perf Part, if in that mode
    buf[2] = 0x00;
    buf[3] = addr;
    buf[4] = val;
    if (open_ports() == EXIT_FAILURE) return;
    if (sysex_update(&buf[0],5) == EXIT_FAILURE) {
      close_ports(); 
      return;
    }
    close_ports();
    state_table->patch_modified = true;
  }
}	// end setPatchSingleValue

void JVlibForm::Patch_setPatchMax() {
  // set the Maximum possible value for Patch Number based on the current Patch Group
  switch(Patch_Group_select->currentIndex()) {
    case 0:     // User
      Patch_Number_select->setMaximum(128);
      break;
    case 1:     // Exp A
      Patch_Number_select->setMaximum(255);
      break;
    case 2:     // PresetA
      Patch_Number_select->setMaximum(128);
      break;
    case 3:     // PresetB
      Patch_Number_select->setMaximum(128);
      break;
    case 4:     // PresetC
      Patch_Number_select->setMaximum(128);
      break;
    case 5:     // PresetD
      Patch_Number_select->setMaximum(128);
      break;
    case 6:     // Exp B
      Patch_Number_select->setMaximum(255);
      break;
    case 7:     // Exp C
      Patch_Number_select->setMaximum(255);
      break;
    default:
      Patch_Number_select->setMaximum(128);
      break;
  }
}       // end Patch_setPatchMax
