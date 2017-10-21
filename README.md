# MOV MetaEdit

MOV MetaEdit is a tool that supports the embedding and editing of metadata in MOV (Apple QuickTime) or MP4 (ISO/IEC 14496-14 a.k.a. MPEG-4 Part 14) files. It is currently focused on Universal Ad ID metadata but could be expanded on request.

## MOV MetaEdit features

- Embedding and editing of Universal Ad ID metadata in MOV (Apple QuickTime) or MP4 (ISO/IEC 14496-14 a.k.a. MPEG-4 Part 14) files.
- Embedding and editing of PAR (Pixel Aspect Ratio), command line only.

## Universal Ad ID editor

After having drag and dropped (or from the "File" menu) some files, the Universal Ad ID view displays information (Universal Ad ID registry and Universal Ad ID value) about each file in a table.  
Then it is possible to edit Universal Ad ID registry and Universal Ad ID value, respecting standard Ad-ID format in case of Ad-ID registry, and save this metadata.

A command line version is also available.

MediaInfo can extract such metadata.

## Pixel Aspect Ratio editor

Currently available only from the command line version, developped for an anonymous sponsor who did not need of a GUI.  
It is possible to embed or edit "pasp" atom as well as the width scale in the "tkhd" atom.

## License

MIT license