#!/usr/bin/env python3
"""Static checks for the Emby server dialog."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "UI" / "dialogs" / "embyserverdialog.h"
SOURCE = ROOT / "UI" / "dialogs" / "embyserverdialog.cpp"
PRO = ROOT / "KikoPlay.pro"
DOC = ROOT / "docs" / "emby-integration.md"


def test_dialog_exposes_config_and_accept_validation():
    header = HEADER.read_text(encoding="utf-8")
    source = SOURCE.read_text(encoding="utf-8")
    assert "struct EmbyServerConfig" in header
    assert "const EmbyServerConfig &serverConfig() const" in header
    assert "void EmbyServerDialog::onAccept()" in source
    assert "Server URL is invalid" in source


def test_dialog_masks_secrets_and_supports_api_key_mode():
    source = SOURCE.read_text(encoding="utf-8")
    assert source.count("setEchoMode(QLineEdit::Password)") >= 2
    assert "Use API key" in source
    assert "updateAuthMode" in source
    assert "apiKeyEdit->setEnabled(apiKeyMode)" in source


def test_dialog_normalizes_url_before_accepting_config():
    source = SOURCE.read_text(encoding="utf-8")
    assert "EmbyClient::normalizeServerUrl(urlEdit->text())" in source


def test_dialog_is_registered_and_documented():
    pro = PRO.read_text(encoding="utf-8")
    doc = DOC.read_text(encoding="utf-8")
    assert "UI/dialogs/embyserverdialog.cpp" in pro
    assert "UI/dialogs/embyserverdialog.h" in pro
    assert "Second step scope" in doc
    assert "EmbyServerDialog" in doc
