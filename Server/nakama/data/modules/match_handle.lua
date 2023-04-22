local nk = require("nakama")

--[[
 Copyright 2018 The Nakama Authors

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
--]]

local du = require("debug_utils")

local OPCODE_JOIN = 100
local OPCODE_LEAVE = 101

local OPCODE_PLAYER_READY = 200
local OPCODE_MATCH_START = 201
local OPCODE_MATCH_END = 202

local OPCODE_PLAYER_SET_WEAPON = 301
local OPCODE_PLAYER_SET_SKILL = 302
local OPCODE_PLAYER_SET_PERK = 303

local OPCODE_PLAYER_VOTE_MAP = 401

local OPCODE_PLAYER_CHANGE_TEAM = 501

local PLAYER_STATUS_PREPARE = 0
local PLAYER_STATUS_READY = 1

local MATCH_STATE_WAITINGTOSTART = "MATCH_STATE_WAITINGTOSTART"
local MATCH_STATE_INPROGRESS = "MATCH_STATE_INPROGRESS"

--[[
Called when a match is created as a result of nk.match_create().

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  execution_mode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match"
}

Params is the optional arbitrary second argument passed to `nk.match_create()`, or `nil` if none was used.

Expected return these values (all required) in order:
1. The initial in-memory state of the match. May be any non-nil Lua term, or nil to end the match.
2. Tick rate representing the desired number of match loop calls per second. Must be between 1 and 30, inclusive.
3. A string label that can be used to filter matches in listing operations. Must be between 0 and 256 characters long.
--]]
local function match_init(context, params)
  local state = {
    debug = (params and params.debug) or false,
    teamA = {},
    teamB = {},
    presences = {},
    players_info = {},
    player_num = 0,
    max_players = (params and params.max_players) or 6
  }
  if state.debug then
    print("match init context:\n" .. du.print_r(context) .. "match init params:\n" .. du.print_r(params))
  end

  local tick_rate = 1 -- per sec
  local label = (params and params.label) or "3v3"

  return state, tick_rate, label
end

--[[
Called when a user attempts to join the match using the client's match join operation.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  execution_mode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match",
  match_label = "the label string returned from match_init",
  match_tick_rate = 1 -- the tick rate returned by match_init
}

Dispatcher exposes useful functions to the match. Format:
{
  broadcast_message = function(op_code, data, presences, sender),
    -- numeric message op code
    -- a data payload string, or nil
    -- list of presences (a subset of match participants) to use as message targets, or nil to send to the whole match
    -- a presence to tag on the message as the 'sender', or nil
  match_kick = function(presences)
    -- a list of presences to remove from the match
  match_label_update = function(label)
    -- a new label to set for the match
}

Tick is the current match tick number, starts at 0 and increments after every match_loop call. Does not increment with
calls to match_join_attempt, match_join, match_leave, match_terminate, or match_signal.

State is the current in-memory match state, may be any Lua term except nil.

Presence is the user attempting to join the match. Format:
{
  user_id: "user unique ID",
  session_id: "session ID of the user's current connection",
  username: "user's unique username",
  node: "name of the Nakama node the user is connected to"
}

Metadata is an optional set of arbitrary key-value pairs received from the client. These may contain information
the client wishes to supply to the match handler in order to process the join attempt, for example: authentication or
match passwords, client version information, preferences etc. Format:
{
  key: "value"
}

Expected return these values (all required) in order:
1. An (optionally) updated state. May be any non-nil Lua term, or nil to end the match.
2. Boolean true if the join attempt should be allowed, false otherwise.
--]]
local function match_join_attempt(context, dispatcher, tick, state, presence, metadata)
  if state.debug then
    print("match join attempt:\n" .. du.print_r(presence))
    print("match join attempt metadata:\n" .. du.print_r(metadata))
  end

  if state.match_state == MATCH_STATE_WAITINGTOSTART or state.match_state == MATCH_STATE_INPROGRESS then
    return {message = "match in progress, failed to join in."}, false
  end

  if state.player_num >= state.max_players then
    return {message = "not extra seat in this rooom, failed to join in."}, false
  end

  return state, true
end

--[[
Called when one or more users have successfully completed the match join process after their match_join_attempt returns
`true`. When their presences are sent to this function the users are ready to receive match data messages and can be
targets for the dispatcher's `broadcast_message` function.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  execution_mode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match",
  match_label = "the label string returned from match_init",
  match_tick_rate = 1 -- the tick rate returned by match_init
}

Dispatcher exposes useful functions to the match. Format:
{
  broadcast_message = function(op_code, data, presences, sender),
    -- numeric message op code
    -- a data payload string, or nil
    -- list of presences (a subset of match participants) to use as message targets, or nil to send to the whole match
    -- a presence to tag on the message as the 'sender', or nil
  match_kick = function(presences)
    -- a list of presences to remove from the match
  match_label_update = function(label)
    -- a new label to set for the match
}

Tick is the current match tick number, starts at 0 and increments after every match_loop call. Does not increment with
calls to match_join_attempt, match_join, match_leave, match_terminate, or match_signal.

State is the current in-memory match state, may be any Lua term except nil.

Presences is a list of users that have joined the match. Format:
{
  {
    user_id: "user unique ID",
    session_id: "session ID of the user's current connection",
    username: "user's unique username",
    node: "name of the Nakama node the user is connected to"
  },
  ...
}

Expected return these values (all required) in order:
1. An (optionally) updated state. May be any non-nil Lua term, or nil to end the match.
--]]
local function match_join(context, dispatcher, tick, state, presences)
  if state.debug then
    print("match join:\n" .. du.print_r(presences) .. du.print_r(state))
  end
  
  local message = {}
  for _, presence in ipairs(presences) do
    state.presences[presence.session_id] = presence

    player_info = {}
    player_info.status = PLAYER_STATUS_PREPARE
    player_info.weapons = {}
    player_info.skills = {}
    player_info.perks = {}
    player_info.vote_map = 0

    state.players_info[presence.session_id] = player_info
    state.player_num = state.player_num + 1

    if table.getn(state.teamA) <= table.getn(state.teamB) then
      table.insert(state.teamA, presence.session_id)
    else
      table.insert(state.teamB, presence.session_id)
    end
  end

  for session_id, presence in pairs(state.presences) do
    local data = {}
    data.session_id = presence.session_id
    data.user_id = presence.user_id
    data.username = presence.username
    data.status = state.players_info[presence.session_id].status

    data.team_id = "teamA"
    for _, id in pairs(state.teamB) do
      if id == data.session_id then
        data.team_id = "teamB"
      end
    end
    table.insert(message, data)
  end

  dispatcher.broadcast_message(OPCODE_JOIN, nk.json_encode(message))

  return state
end

--[[
Called when one or more users have left the match for any reason, including connection loss.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  execution_mode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match",
  match_label = "the label string returned from match_init",
  match_tick_rate = 1 -- the tick rate returned by match_init
}

Dispatcher exposes useful functions to the match. Format:
{
  broadcast_message = function(op_code, data, presences, sender),
    -- numeric message op code
    -- a data payload string, or nil
    -- list of presences (a subset of match participants) to use as message targets, or nil to send to the whole match
    -- a presence to tag on the message as the 'sender', or nil
  match_kick = function(presences)
    -- a list of presences to remove from the match
  match_label_update = function(label)
    -- a new label to set for the match
}

Tick is the current match tick number, starts at 0 and increments after every match_loop call. Does not increment with
calls to match_join_attempt, match_join, match_leave, match_terminate, or match_signal.

State is the current in-memory match state, may be any Lua term except nil.

Presences is a list of users that have left the match. Format:
{
  {
    user_id: "user unique ID",
    session_id: "session ID of the user's current connection",
    username: "user's unique username",
    node: "name of the Nakama node the user is connected to"
  },
  ...
}

Expected return these values (all required) in order:
1. An (optionally) updated state. May be any non-nil Lua term, or nil to end the match.
--]]
local function match_leave(context, dispatcher, tick, state, presences)
  if state.debug then
    print("match leave:\n" .. du.print_r(presences))
  end

  for _, presence in ipairs(presences) do
    state.presences[presence.session_id] = nil
    state.players_info[presence.session_id] = nil
    state.player_num = state.player_num - 1

    for i, v in pairs(state.teamA) do
      if v == presence.session_id then
        table.remove(state.teamA, i)
        break
      end
    end

    for i, v in pairs(state.teamB) do
      if v == presence.session_id then
        table.remove(state.teamB, i)
        break
      end
    end
  end

  return state
end

--[[
Called on an interval based on the tick rate returned by match_init.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  executionMode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match",
  match_label = "the label string returned from match_init",
  match_tick_rate = 1 -- the tick rate returned by match_init
}

Dispatcher exposes useful functions to the match. Format:
{
  broadcast_message = function(op_code, data, presences, sender),
    -- numeric message op code
    -- a data payload string, or nil
    -- list of presences (a subset of match participants) to use as message targets, or nil to send to the whole match
    -- a presence to tag on the message as the 'sender', or nil
  match_kick = function(presences)
    -- a list of presences to remove from the match
  match_label_update = function(label)
    -- a new label to set for the match
}

Tick is the current match tick number, starts at 0 and increments after every match_loop call. Does not increment with
calls to match_join_attempt, match_join, match_leave, match_terminate, or match_signal.

State is the current in-memory match state, may be any Lua term except nil.

Messages is a list of data messages received from users between the previous and current ticks. Format:
{
  {
    sender = {
      user_id: "user unique ID",
      session_id: "session ID of the user's current connection",
      username: "user's unique username",
      node: "name of the Nakama node the user is connected to"
    },
    op_code = 1, -- numeric op code set by the sender.
    data = "any string data set by the sender" -- may be nil.
  },
  ...
}

Expected return these values (all required) in order:
1. An (optionally) updated state. May be any non-nil Lua term, or nil to end the match.
--]]
local function match_loop(context, dispatcher, tick, state, messages)
  --if state.debug then
    --print("match " .. context.match_id .. " tick " .. tick)
    --print("match " .. context.match_id .. " messages:\n" .. du.print_r(messages))
  --end
  if tick > 600 and state.player_num == 0 then
     return nil -- terminal match
  end

  if state.match_state == MATCH_STATE_WAITINGTOSTART then
    local payload = {}
    payload.server_addr = state.server_addr
    payload.server_port = state.server_port
    dispatcher.broadcast_message(OPCODE_MATCH_START, nk.json_encode(payload))
    state.match_state = MATCH_STATE_INPROGRESS
    print("server change to MATCH_STATE_INPROGRESS")
  end

  for _, m in ipairs(messages) do
    print(string.format("Received %s from %s", m.data, m.sender.username))
    local decoded = nk.json_decode(m.data)

    -- for k, v in pairs(decoded) do
    --   print(string.format("Key %s contains value %s", k, v))
    -- end

    if m.op_code == OPCODE_PLAYER_READY then
      local message = {}
      message.user_id = state.presences[m.sender.session_id].user_id
      message.user_name = state.presences[m.sender.session_id].username

      state.players_info[m.sender.session_id].status = PLAYER_STATUS_READY

      local ready_players_num = 0
      for session_id, presence in pairs(state.presences) do
        --print("presence status: " .. session_id .. ", " .. state.players_info[session_id])
        if state.players_info[session_id].status == PLAYER_STATUS_READY then
          ready_players_num = ready_players_num + 1
        end
      end

      -- broadcast all players that player is ready
      dispatcher.broadcast_message(OPCODE_PLAYER_READY, nk.json_encode(message))

      -- start ue4 server immediately
      if ready_players_num == state.player_num and table.getn(state.teamA) > 0 and table.getn(state.teamB) > 0 then

        vote_maps = {}
        for player_id, player_info in pairs(state.players_info) do
          map_id = player_info.vote_map
          vote_maps[map_id] = vote_maps[map_id] and vote_maps[map_id] + 1 or 1
        end

        best_map = 0
        max_votes = 0
        for map_id, votes in pairs(vote_maps) do
          if votes >= max_votes then
            max_votes = votes
            best_map = map_id
          end
        end

        map_name = "map" .. best_map
        print(context.env[map_name])

        server_info = nk.start_ue4_server(context.match_id:gsub(".nakama", ""), context.env[map_name])
        state.server_addr = context.env.ue4_server_ip
        state.server_port = server_info.ue4_server_port
        state.match_state = MATCH_STATE_WAITINGTOSTART
      end
    elseif m.op_code == OPCODE_PLAYER_SET_WEAPON then
      -- request set weapon
      dispatcher.broadcast_message(OPCODE_PLAYER_SET_WEAPON, m.data)
      local weapon_slot = decoded.weapon_slot
      local weapon_id = decoded.weapon_id
      state.players_info[m.sender.session_id].weapons[weapon_slot] = weapon_id
    elseif m.op_code == OPCODE_PLAYER_SET_SKILL then
      -- request set skill
      dispatcher.broadcast_message(OPCODE_PLAYER_SET_SKILL, m.data)
      local skill_slot = decoded.skill_slot
      local skill_id = decoded.skill_id
      state.players_info[m.sender.session_id].skills[skill_slot] = skill_id
    elseif m.op_code == OPCODE_PLAYER_SET_PERK then
      -- request set perk
      dispatcher.broadcast_message(OPCODE_PLAYER_SET_PERK, m.data)
      local perk_slot = decoded.perk_slot
      local perk_id = decoded.perk_id
      state.players_info[m.sender.session_id].perks[perk_slot] = perk_id
    elseif m.op_code == OPCODE_PLAYER_VOTE_MAP then
      -- request set map
      local map_id = decoded.map_id
      state.players_info[m.sender.session_id].vote_map = map_id
    elseif m.op_code == OPCODE_PLAYER_CHANGE_TEAM then
      local old_team = decoded.old_team_id
      local new_team = decoded.new_team_id

      local old_team_table = old_team == 0 and state.teamA or state.teamB
      local new_team_table = new_team == 0 and state.teamA or state.teamB

      if table.getn(new_team_table) < state.max_players / 2 then
        table.insert(new_team_table, m.sender.session_id)

        for i, v in pairs(old_team_table) do
          if v == m.sender.session_id then
            table.remove(old_team_table, i)
            break
          end
        end

        local message = {}
        message.user_id = state.presences[m.sender.session_id].user_id
        message.user_name = state.presences[m.sender.session_id].username
        message.old_team = old_team
        message.new_team = new_team

        dispatcher.broadcast_message(OPCODE_PLAYER_CHANGE_TEAM, nk.json_encode(message))
      end
    end
  end

  return state
end

--[[
Called when the server begins a graceful shutdown process. Will not be called if graceful shutdown is disabled.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  executionMode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match",
  match_label = "the label string returned from match_init",
  match_tick_rate = 1 -- the tick rate returned by match_init
}

Dispatcher exposes useful functions to the match. Format:
{
  broadcast_message = function(op_code, data, presences, sender),
    -- numeric message op code
    -- a data payload string, or nil
    -- list of presences (a subset of match participants) to use as message targets, or nil to send to the whole match
    -- a presence to tag on the message as the 'sender', or nil
  match_kick = function(presences)
    -- a list of presences to remove from the match
  match_label_update = function(label)
    -- a new label to set for the match
}

Tick is the current match tick number, starts at 0 and increments after every match_loop call. Does not increment with
calls to match_join_attempt, match_join, match_leave, match_terminate, or match_signal.

State is the current in-memory match state, may be any Lua term except nil.

Grace Seconds is the number of seconds remaining until the server will shut down.

Expected return these values (all required) in order:
1. An (optionally) updated state. May be any non-nil Lua term, or nil to end the match.
--]]
local function match_terminate(context, dispatcher, tick, state, grace_seconds)
  if state.debug then
    print("match " .. context.match_id .. " tick " .. tick)
    print("match " .. context.match_id .. " grace_seconds " .. grace_seconds)
  end
  return state
end

--[[
Called when the match handler receives a runtime signal.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  executionMode = "Match",
  match_id = "client-friendly match ID, can be shared with clients and used in match join operations",
  match_node = "name of the Nakama node hosting this match",
  match_label = "the label string returned from match_init",
  match_tick_rate = 1 -- the tick rate returned by match_init
}

Dispatcher exposes useful functions to the match. Format:
{
  broadcast_message = function(op_code, data, presences, sender),
    -- numeric message op code
    -- a data payload string, or nil
    -- list of presences (a subset of match participants) to use as message targets, or nil to send to the whole match
    -- a presence to tag on the message as the 'sender', or nil
  match_kick = function(presences)
    -- a list of presences to remove from the match
  match_label_update = function(label)
    -- a new label to set for the match
}

Tick is the current match tick number, starts at 0 and increments after every match_loop call. Does not increment with
calls to match_join_attempt, match_join, match_leave, match_terminate, or match_signal.

State is the current in-memory match state, may be any Lua term except nil.

Data is arbitrary input supplied by the runtime caller of the signal.

Expected return these values (all required) in order:
1. An (optionally) updated state. May be any non-nil Lua term, or nil to end the match.
1. Arbitrary data to return to the runtime caller of the signal. May be a string, or nil.
--]]
local function match_signal(context, dispatcher, tick, state, data)
  if state.debug then
    print("match " .. context.match_id .. " tick " .. tick)
    print("match " .. context.match_id .. " data " .. data)
  end
  return state, "signal received: " .. data
end

-- Match modules must return a table with these functions defined. All functions are required.
return {
  match_init = match_init,
  match_join_attempt = match_join_attempt,
  match_join = match_join,
  match_leave = match_leave,
  match_loop = match_loop,
  match_terminate = match_terminate,
  match_signal = match_signal
}