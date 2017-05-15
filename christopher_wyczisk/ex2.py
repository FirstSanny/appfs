import sys, csv
from lxml import etree, objectify
from lxml.etree import XMLSchemaParseError, XMLSyntaxError

# Auch diesen Code hat Christopher Wyczisk runtergesaut, da er sich
# aufgrund einer Razzia in seinem Buero bei Toll Collect nicht aufs Coden
# konzentrieren konnte.

# Start z.B. per: python ex2.py measured-1.0.0.2017-02-03.b0050c5c8deb1db59c7b2644414b079d.xml measured-1-1-0.xsd result.csv

class MeasuredDto(object):
    
    def __init__(self, datumAlsString, hourAlsString, wertAlsString):
         self.datum = datumAlsString
         self.hour = hourAlsString
         self.wert = wertAlsString
    
    def getAsSemikolonString(self):
        string = self.datum + ";" + self.hour + ";" + self.wert
        return string
         
         
class EtlProzess(object):
    
    def __init__(self, xmlInputFilename, xslInputFilename, csvOutputFilename):
        self.__xmlInputFilename = xmlInputFilename
        self.__xslInputFilename = xslInputFilename
        self.__csvOutputFilename =csvOutputFilename
        self.__messureds = []
    
    def readXml(self):
        try:
            schema = self.__readXsd()
            xml = self.__readXml(schema)
            self.__readMessureds(xml)
        except (XMLSchemaParseError, XMLSyntaxError) as error:
            sys.stderr = open('error.log', 'w')
            sys.stderr.write(str(error))
            print("Warnung: Probleme wurden bemerkt und im error.log beschrieben.")
            xml = self.__readXml()
            self.__readMessureds(xml)
        
    def writeCsv(self):
        fobj = open(self.__csvOutputFilename, "w")
        csvWriter = csv.writer(fobj, delimiter=';')
        for measured in self.__messureds:
            #print("Speichere: " + measured.getAsSemikolonString())
            csvWriter.writerow([measured.datum, measured.hour, measured.wert])
        fobj.close()
        
    def __readXsd(self):
        schema = etree.parse(self.__xslInputFilename)
        return etree.XMLSchema(schema)
        
    def __readXml(self, schema=None):
        parser = etree.XMLParser(remove_comments=True, schema=schema)
        tree = objectify.parse(self.__xmlInputFilename, parser=parser)
        return tree.getroot()
        
    def __readMessureds(self, xml):
        nsmap = xml.nsmap
        knotenMenge = xml.find("{" + nsmap[None] + "}gasDay")
        datum = knotenMenge.get('date')
        hour = knotenMenge.get('gasDayStartHourInUTC')
        for knoten in knotenMenge:
            for tag in knoten:
                wert = tag.find("{" + nsmap[None] + "}amountOfPower").get('value')
                self.__messureds.append(MeasuredDto(datum, int(hour) + int(tag.get('hour')), wert))
        
        
        
# Einlesen der Konsoleneingabe
xmlInputFilename = sys.argv[1]
xslInputFilename = sys.argv[2]
csvOutputFilename = sys.argv[3]

# Bau des Etl Objekts und Datenuebertragung von .xml zu .csv
etl = EtlProzess(xmlInputFilename, xslInputFilename, csvOutputFilename)
print("Einlesen von " + xmlInputFilename + "...")
etl.readXml()
print(xmlInputFilename + " wurde gelesen. Jetzt wird das csv erstellt...")
etl.writeCsv()
print("Fertig! Das csv File " + csvOutputFilename + " wurde erstellt.")




