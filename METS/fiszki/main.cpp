#include "cards.h"


int main(int argc, char* argv[])
{
	QTextStream out(stdout);
	if (argc < 2) {
		out << "Usage: fiszki [<directory> | <csv file>] [djvu index file]\n";
		return -1;
	}

	QLocale loc = QLocale::system();
	QString status = QString("Fiszki 0.6: locale %1\n")
						  .arg(QLocale::languageToString(loc.language()));

	out << status;

	Cards cards;

	QString path = QString::fromAscii(argv[1]);
	QString csvFilename;
	QString maleksFilename;
	QString sqlFilename;
	QDir dir(QString::fromAscii(argv[1]));
	if (dir.exists()) {
		foreach (QString filename, dir.entryList(QStringList("*.*"))) {
			QFileInfo entry(dir, filename);
			if (entry.isFile())
				cards.parse(entry.absoluteFilePath());
		}
		csvFilename = dir.dirName() + ".csv";
		maleksFilename = dir.dirName() + ".maleks";
		sqlFilename =  dir.dirName() + ".sql";
	}
	else if (QFile(path).exists()) {
		cards.readCsv(path);
		csvFilename = QFileInfo(path).baseName() + "-updated.csv";
		maleksFilename = QFileInfo(path).baseName() + ".maleks";
		sqlFilename = QFileInfo(path).baseName() + ".sql";
	}
	else {
		out << "Error: file or directory does not exist.\n";
		return -2;
	}

	cards.sort();
	cards.setUniqueFiles();
	cards.saveCsv(csvFilename);
	cards.saveMaleks(maleksFilename);
	if (argc > 2)
		cards.saveSql(QString::fromLocal8Bit(argv[2]), sqlFilename);


}
