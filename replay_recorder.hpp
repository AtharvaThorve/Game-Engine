#pragma once
#include "event_manager.hpp"
#include "timeline.hpp"
#include <vector>

class ReplayRecorder : public EventHandler {
public:
  ReplayRecorder(Timeline *timeline);
  void on_event(const Event &event) override;

private:
  Timeline *timeline;

  bool is_recording = false;
  int64_t recording_start_time = 0;
  int64_t replay_start_time = 0;

  std::vector<Event> recorded_events;

  const size_t start_recording_hash =
      std::hash<std::string>{}("start_recording");
  const size_t stop_recording_hash = std::hash<std::string>{}("stop_recording");
  const size_t play_recording_hash = std::hash<std::string>{}("play_recording");

  void start_recording();
  void stop_recording();
  void play_recording();
  void record_event(const Event &event);
};