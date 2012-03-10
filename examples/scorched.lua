-- Basic Scorched Earth clone

NUM_PLAYERS = 3

function setup()
    setup_terrain()
    setup_players()
end

function setup_terrain()
    terrain = {}
    local periods = {}
    local amplitudes = {}
    local nwaves = 10
    for i = 1, nwaves do
        periods[i] = math.random()*0.1 + 0.01
        amplitudes[i] = math.random() * 50
    end
    for i = 0, WIDTH-1 do
        local h = 200
        for j = 1, nwaves do
            h = h + math.sin(i*periods[j])*amplitudes[j]
        end
        terrain[i] = h
    end
end

function setup_players()
    players = {}
    for i = 1, NUM_PLAYERS do
        local player = {}
        player.x = math.random(10, WIDTH-10)
        player.y = terrain[player.x]
        player.angle = 30
        player.r = 255
        player.g = 0
        player.b = 0
        for x = player.x - 8, player.x + 8 do
            if terrain[x] >= player.y then
                terrain[x] = player.y-1
            end
        end
        players[i] = player
    end
end

function draw()
    draw_terrain()
    draw_players()
end

function draw_terrain()
    background(30, 30, 200)
    fill(20, 150, 20, 1.0)
    for i = 0, WIDTH-1 do
        line(i, 0, i, terrain[i])
    end
end

function draw_players()
    for i, player in ipairs(players) do
        fill(player.r, player.g, player.b, 1)
        rect(player.x-6, player.y, 12, 8)
        local l = 13
        line(player.x, player.y+6,
             player.x + l*math.cos(math.rad(player.angle)),
             player.y+6 + l*math.sin(math.rad(player.angle)))
    end
end
