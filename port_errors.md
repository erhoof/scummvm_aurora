# Device findings

## ROT-001 — reversed landscape direction

Reported by user after first tablet launch: content is upside down, using 90°
where 270° is needed. Portrait-panel landscape mapping and its inverted partner
were exchanged; buffer hint and touch use the same rotation mode. Release 2
built, signed and validated. Status: awaiting tablet confirmation.

## ROT-002 — reversed compositor hint

User confirms release 2 image is correct, but Aurora hint is upside down.
Invert only the Wayland hint's quarter turns (ScummVM 270° -> Wayland 90°,
ScummVM 90° -> Wayland 270°). Texture and touch mappings remain unchanged.
Release 3 prepared. Status: awaiting tablet confirmation.

ROT-001 image correction confirmed by user; closed.

ROT-002: closed following user's overall positive feedback after release 5
("Looks like everything is good"). No further rotation fault reported.
