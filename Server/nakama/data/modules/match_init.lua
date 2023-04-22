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

local nk = require("nakama")

--[[
Called when the matchmaker has found a match for some set of users.

Context represents information about the match and server, for information purposes. Format:
{
  env = {}, -- key-value data set in the runtime.env server configuration.
  execution_mode = "Matchmaker",
}

Matchmaker users will contain a table array of the users that have matched together and their properties. Format:
{
  {
    presence: {
      user_id: "user unique ID",
      session_id: "session ID of the user's current connection",
      username: "user's unique username",
      node: "name of the Nakama node the user is connected to"
    },
    properties: {
      foo: "any properties the client set when it started its matchmaking process",
      baz: 1.5
    }
  },
  ...
}

Expected to return an authoritative match ID for a match ready to receive these users, or `nil` if the match should
proceed through the peer-to-peer relayed mode.
--]]

local function matchmaker_matched(context, matchmaker_users)
  if #matchmaker_users <= 0 then
    return nil
  end

  local min_players_num = matchmaker_users[1].properties["minPlayers"]
  local max_players_num = matchmaker_users[1].properties["maxPlayers"]

  if #matchmaker_users < min_players_num or #matchmaker_users > max_players_num then
    return nil
  end

  for i=1,#matchmaker_users do
    if matchmaker_users[i].properties["mode"] ~= "authoritative" then
      return nil
    end
  end

  return nk.match_create("match_handle", {debug = true, expected_users = matchmaker_users, label = tostring(max_players_num), max_players = max_players_num})
end

nk.register_matchmaker_matched(matchmaker_matched)