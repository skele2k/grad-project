Use parameter with name of the gz file to process without adding its extension .gz to the name.
ex. ssdtrace-00.gz -> ./tracer ssdtrace-00

Extracting field [Timestamp] [Trace Action] [Operation Type]
[SectorNumber] [I/O Size] from format like below
259,2    7   141911   203.947413674  3276  Q  WS 499472400 + 8 [jbd2/nvme0n1-8]

Extracting field [Timestamp] [Trace Action] [Operation Type]
[SectorNumber] from format like below
259,2    2   151411   241.991304422  4047  C WFS 499500664 [0]

used grep C to extract ones only Completing trace operation.
used grep W to extract ones only ones related to Writing operation.

"maybe [Trace Action] and [Operation Type] is not needed.
Would I/O size matter?"
Please give some opinion for any field that is not included above needs to be added to our data extracting.
Those information can be found from README