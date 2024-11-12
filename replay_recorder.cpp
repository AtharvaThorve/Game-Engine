#include "replay_recorder.hpp"

ReplayRecorder::ReplayRecorder(EventManager *em, Timeline *timeline)
    : em(em), timeline(timeline) {}

void ReplayRecorder::on_event(const Event &event) {
  if (event.type == start_recording_hash) {
    start_recording();
  } else if (event.type == stop_recording_hash) {
    stop_recording();
  } else if (event.type == play_recording_hash) {
    play_recording();
  } else {
    record_event(event);
  }
}

void ReplayRecorder::start_recording() {
  is_recording = true;
  recording_start_time = timeline->getTime();
  recorded_events.clear();
}

void ReplayRecorder::stop_recording() { is_recording = false; }

void ReplayRecorder::record_event(const Event &event) {
  if (is_recording) {
    Event copy_event = event;
    copy_event.timestamp -= recording_start_time;
    recorded_events.push_back(copy_event);
  }
}

void ReplayRecorder::play_recording() {
  if (!is_recording) {
    replay_start_time = timeline->getTime();
    for (auto &event : recorded_events) {
      Event replay_event = event;
      replay_event.timestamp += replay_start_time;
      em->raise_event(replay_event);
    }
  } else {
    std::cerr << "Recording is still running, stop it before replaying it"
              << std::endl;
  }
}
