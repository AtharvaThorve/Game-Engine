function runScript() {
    print(player.position.x);
    print(player.position.y);

    const params = {
        player: player,
    };

    raise_event("death", params);
}

runScript();
