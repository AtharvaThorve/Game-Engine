#pragma once
#include "entity_manager.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include <unordered_map>
#include <vector>

class ReplayRecorder : public EventHandler {
public:
  ReplayRecorder(Timeline *timeline,
                 std::vector<std::shared_ptr<EntityManager>> &entityManagers);
  void on_event(const Event &event) override;

private:
  Timeline *timeline;
  std::vector<std::shared_ptr<EntityManager>> &entityManagers;

  bool is_recording = false;
  int64_t recording_start_time = 0;
  int64_t replay_start_time = 0;

  std::vector<Event> recorded_events;
  std::unordered_map<int64_t, std::unordered_map<std::string, std::string>>
      initial_states;
  std::unordered_map<int64_t, std::unordered_map<std::string, std::string>>
      final_states;

  const size_t start_recording_hash =
      std::hash<std::string>{}("start_recording");
  const size_t stop_recording_hash = std::hash<std::string>{}("stop_recording");
  const size_t play_recording_hash = std::hash<std::string>{}("play_recording");
  const size_t replay_complete_hash =
      std::hash<std::string>{}("replay_complete");

  std::shared_ptr<Entity>find_entity_by_id(int64_t id) const;

  void start_recording();
  void stop_recording();
  void play_recording();
  void replay_complete();
  void record_event(const Event &event);
};