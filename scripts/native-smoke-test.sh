#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build-qa"
TMP="$(mktemp -d)"
cleanup(){ rm -rf "$TMP"; }
trap cleanup EXIT

cat > "$TMP/ForgeProject.json" <<JSON
{
  "projectId": "professional-smoke",
  "projectName": "ProfessionalSmoke",
  "engineVersion": "3.1.0",
  "projectVersion": "1.0.0",
  "defaultScene": "Scenes/Main.forgeScene",
  "template": "3D",
  "targetPlatforms": ["Linux"],
  "enabledModules": ["Core","Renderer","3D"],
  "rendering": {"profile": "High"},
  "physics": {"backend": "ForgePhysics"},
  "audio": {"enabled": true},
  "networking": {"enabled": false}
}
JSON
mkdir -p "$TMP/Scenes" "$TMP/Assets"
cat > "$TMP/Scenes/Main.forgeScene" <<JSON
{
  "sceneVersion": 2,
  "entities": [
    {"id":1,"name":"MainCamera","type":"Camera","position":[0,2,8],"scale":[1,1,1]},
    {"id":2,"name":"Cube","type":"Mesh","position":[0,0,0],"scale":[1,1,1]},
    {"id":3,"name":"DirectionalLight","type":"Light","position":[4,6,2],"scale":[1,1,1]}
  ]
}
JSON

run_alive(){
  local label="$1"; shift
  set +e
  timeout 4s "$@" >/tmp/forge-smoke.log 2>&1
  local rc=$?
  set -e
  if [[ $rc -eq 124 ]]; then
    echo "PASS: $label stayed alive for smoke window"
  else
    echo "FAIL: $label exited with code $rc"
    cat /tmp/forge-smoke.log
    return 1
  fi
}

if command -v xvfb-run >/dev/null 2>&1; then
  run_alive "ForgeEngine native editor" xvfb-run -a "$BUILD/ForgeEngine"
  run_alive "ForgeRuntime native runtime" xvfb-run -a "$BUILD/ForgeRuntime" "$TMP"
else
  echo "SKIP: xvfb-run is not installed; native GUI smoke test unavailable on this host."
fi
