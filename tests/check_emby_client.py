#!/usr/bin/env python3
"""Static checks for the initial Emby client foundation."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "MediaServer" / "Emby" / "embyclient.h"
SOURCE = ROOT / "MediaServer" / "Emby" / "embyclient.cpp"
DOC = ROOT / "docs" / "emby-integration.md"
PRO = ROOT / "KikoPlay.pro"


def test_emby_client_exposes_minimum_flow():
    header = HEADER.read_text(encoding="utf-8")
    for symbol in [
        "authenticateByName",
        "systemInfo",
        "publicSystemInfo",
        "views",
        "items",
        "item",
        "playbackInfo",
        "streamUrl",
    ]:
        assert symbol in header


def test_token_is_sent_in_headers_not_stream_url():
    source = SOURCE.read_text(encoding="utf-8")
    stream_url_body = source.split("QUrl EmbyClient::streamUrl", 1)[1].split("QString EmbyClient::normalizeServerUrl", 1)[0]
    assert "X-Emby-Token" in source
    assert "Token" not in stream_url_body
    assert "api_key" not in stream_url_body


def test_emby_sources_are_registered_in_qmake_project():
    pro = PRO.read_text(encoding="utf-8")
    assert "MediaServer/Emby/embyclient.cpp" in pro
    assert "MediaServer/Emby/embyclient.h" in pro


def test_integration_notes_describe_stable_emby_paths():
    doc = DOC.read_text(encoding="utf-8")
    assert "emby://serverId/itemId" in doc
    assert "real stream URL should be resolved only when playback starts" in doc
