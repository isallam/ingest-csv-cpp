./ingest_csv -m ../config/phoneMapper.json     -f ../source-data/phones.csv 
./ingest_csv -m ../config/personMapper.json    -f ../source-data/people.csv
./ingest_csv -m ../config/addressMapper.json   -f ../source-data/addresses.csv
./ingest_csv -m ../config/phoneCallMapper.json -p "../source-data/phonecalls*.csv"
