#include "replay_recorder.hpp"

ReplayRecorder::ReplayRecorder(
    Timeline *timeline,
    std::vector<std::shared_ptr<EntityManager>> &entityManagers)
    : timeline(timeline), entityManagers(entityManagers) {}

void ReplayRecorder::on_event(const Event &event) {
  if (event.type == start_recording_hash) {
    start_recording();
  } else if (event.type == stop_recording_hash) {
    stop_recording();
  } else if (event.type == play_recording_hash) {
    play_recording();
  } else if (event.type == replay_complete_hash) {
    replay_complete();
  } else {
    record_event(event);
  }
}

void ReplayRecorder::start_recording() {
  is_recording = true;
  recording_start_time = timeline->getTime();
  recorded_events.clear();
  initial_positions.clear();

  for (const auto &manager : entityManagers) {
    for (const auto &entity : manager->getEntities()) {
      if (entity) {
        initial_positions[entity] = entity->position;
      }
    }
  }
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
    EventManager &em = EventManager::getInstance();

    final_positions.clear();
    for (const auto &manager : entityManagers) {
      for (const auto &entity : manager->getEntities()) {
        if (entity) {
          final_positions[entity] = entity->position;
        }
      }
    }

    // Snap entities to the initial_positions they were at when recording
    // started
    for (const auto &pair : initial_positions) {
      auto entity = pair.first;
      entity->setPosition(pair.second);
      entity->velocity = {0, 0};
      entity->inputAcceleration = {0, 0};
    }

    em.set_replay_only_mode(true);

    for (auto &event : recorded_events) {
      Event replay_event = event;
      replay_event.timestamp += replay_start_time;
      replay_event.parameters["is_replay_event"] = true;
      em.raise_event(replay_event);
    }
  } else {
    std::cerr << "Recording is still running, stop it before replaying it"
              << std::endl;
  }
}

void ReplayRecorder::replay_complete() {
  for (const auto &pair : final_positions) {
    auto entity = pair.first;
    entity->setPosition(pair.second);
    entity->velocity = {0, 0};
    entity->inputAcceleration = {0, 0};
  }
}