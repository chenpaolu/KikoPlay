#!/usr/bin/env python3
"""Lightweight regression checks for LANServer static file path handling."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "LANServer" / "httpserver" / "staticfilecontroller.cpp"


def test_static_file_controller_uses_canonical_docroot_guard():
    source = SOURCE.read_text(encoding="utf-8")
    assert "canonicalDocroot" in source
    assert "requestedFile.canonicalFilePath()" in source
    assert "requestedPath.startsWith(canonicalDocroot + QDir::separator())" in source


def test_percent_encoded_paths_are_decoded_before_guard():
    source = SOURCE.read_text(encoding="utf-8")
    assert "QUrl::fromPercentEncoding(path)" in source
    assert source.index("QUrl::fromPercentEncoding(path)") < source.index("canonicalDocroot")


def test_forbidden_paths_return_403():
    source = SOURCE.read_text(encoding="utf-8")
    assert 'response.setStatus(403,"forbidden")' in source
    assert 'response.write("403 forbidden",true)' in source
