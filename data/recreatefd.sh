objy deletefd -bootfile testfd.boot
objy createfd -fdname testfd
objy do -infile ../config/allschema.txt -boot testfd.boot 
#objy addIndex -name PersonId  -class Person  -attribute id  -bootfile testfd.boot
#objy addIndex -name PhoneNumber  -class Phone  -attribute phone_number -bootfile testfd.boot
# objy importPlacement -infile calldata.pmd -boot testfd.boot


