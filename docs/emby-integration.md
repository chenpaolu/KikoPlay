# Emby integration notes

This is the first implementation step for Emby server support.  The goal is to
keep Emby isolated behind a small client wrapper before wiring it into playlist
UI and playback.

## First step scope

- Add `EmbyClient` as a thin REST wrapper.
- Normalize server URLs in one place.
- Keep authentication tokens in headers instead of embedding them in playback
  URLs or playlist paths.
- Provide methods for the minimum browsing/playback flow:
  - `Users/AuthenticateByName`
  - `System/Info` and `System/Info/Public`
  - `Users/{UserId}/Views`
  - `Users/{UserId}/Items`
  - `Users/{UserId}/Items/{ItemId}`
  - `Items/{ItemId}/PlaybackInfo`
  - `Videos/{ItemId}/stream`

## Next step

The next step should add an Emby server configuration dialog and a playlist
collection type that stores stable `emby://serverId/itemId` identifiers.  The
real stream URL should be resolved only when playback starts, because tokens,
media source IDs, and play sessions can change.
