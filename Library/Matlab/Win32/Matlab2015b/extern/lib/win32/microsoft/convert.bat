mkdir borland
for %%F in (*.lib) do coff2omf %%F borland\%%F