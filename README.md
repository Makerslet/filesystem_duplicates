Tool for searching duplicates in filesystem

## Parameters:

```bash
--h			Show help
--t			Direcroties list for scanning (required)
--e			List of excluding directories (optional, by default is empty)
--l			Level of scanning (optional, by default is not set)
--ms			Minimal file size (optional, by default is 1)
--m			List of file masks (optional, by default is empty)
--bs			Block size for reading files (optional, by default is 4K)
--a			Name of hashing algorithm (optional, by defaulr is crc32, available& crc32, crc16)
```

**Examples**: 

`filesystem_duplicates --t ~ --e ~/projects --l=3 --ms=1024 --bs=1024 --a=crc32`
