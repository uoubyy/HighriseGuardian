local nk = require("nakama")
local du = require("debug_utils")

local function print_env(context, _)
  print("env:\n" .. du.print_r(context.env))
  local response = {
    message = context.env
  }
  return nk.json_encode(response)
end
nk.register_rpc(print_env, "server_rpc.print_env")

local function get_match_info(context, playload)
    local decoded = nk.json_decode(playload)
    local match = nk.match_get(decoded.match_id)
    print("get_match_info:\n" .. du.print_r(match))
    return nk.json_encode(match)
end
nk.register_rpc(get_match_info, "server_rpc.get_match_info")

local function get_match_state(context, playload)
    local decoded = nk.json_decode(playload)
    local state = nk.match_state(decoded.match_id, decoded.node)
    print("get_match_state:\n" .. du.print_r(state))
    return nk.json_encode(state)
end
nk.register_rpc(get_match_state, "server_rpc.get_match_state")

local function create_authoritative_match(_context, payload)
  local decoded = nk.json_decode(payload)
  local params = {
    debug = (decoded and decoded.debug) or true,
    label = tostring(decoded.max_players),
    max_players = decoded.max_players
  }

  local match_id = nk.match_create("match_handle", params)
  return nk.json_encode({ match_id = match_id })
end
nk.register_rpc(create_authoritative_match, "server_rpc.create_authoritative_match")