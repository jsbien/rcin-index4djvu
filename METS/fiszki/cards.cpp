#include "cards.h"

Cards::Cards()
{
}

void Cards::parse(const QString &filename)
{
	Card card;
	if (card.parse(filename))
		m_cards.append(card);
	else qDebug() << "Error parsing" << filename;
}

void Cards::sort()
{
	qStableSort(m_cards);
	for (int i = 0; i < m_cards.count(); i++)
		m_cards[i].setSequence(i+1);
}

void Cards::setUniqueFiles()
{
	QSet<QString> uniqueFilenames;
	QSet<QString> skipFilenames;
	for (int i = 0; i < m_cards.count(); i++) {
		QString unique = m_cards[i].filename().section('_', -1, -1).section('.', 0, 0);
		if (uniqueFilenames.contains(unique))
			skipFilenames.insert(unique);
		else uniqueFilenames.insert(unique);
	}
	for (int i = 0; i < m_cards.count(); i++) {
		QString unique = m_cards[i].filename().section('_', -1, -1).section('.', 0, 0);
		if (!skipFilenames.contains(unique))
			m_cards[i].setUniqueFilename(unique);
	}
}

bool Cards::readCsv(const QString &csvFile)
{
	QFile file(csvFile);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	m_cards.clear();
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream.readLine();
	while (!stream.atEnd()) {
		QString line = stream.readLine().trimmed();
		Card card;
		if (card.parseCvsRow(line))
			m_cards.append(card);
	}
	return m_cards.count() > 0;
}

bool Cards::saveCsv(const QString &csvFile)
{	
	QFile file(csvFile);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << "Filename;Drawer;Base filename;First word;Last word;First word index;Last word index;Sequence;Error message\n";
	for (int i = 0; i < m_cards.count(); i++)
		stream << m_cards[i].csvRow();
	return true;
}

bool Cards::saveMaleks(const QString &maleksFile)
{
	QFile file(maleksFile);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	for (int i = 0; i < m_cards.count(); i++) {
		stream << QString("$ %1\t%2\n").arg(m_cards[i].drawer()).arg(m_cards[i].uniqueFilename());
		stream << "\t$from_names_djvu\t$all\n";
		stream << "$end\n\n";
	}
	return true;
}

bool Cards::saveSql(const QString &djvuIndexFile, const QString &sqlFilename)
{
	QFile indexFile(djvuIndexFile);
	if (!indexFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Failr";
		return false;
	}

	QMap<QString, QString> fileMap;
	QTextStream indexStream(&indexFile);
	while (!indexStream.atEnd()) {
		QString line = indexStream.readLine().trimmed();
		if (line.contains(','))
			fileMap[line.section(',', 0, 0)] = line.section(',', 1);
	}

	qDebug() << "SQLout" << sqlFilename;
	QFile sqlFile(sqlFilename);
	if (!sqlFile.open(QIODevice::WriteOnly))
		return false;
	QTextStream sqlStream(&sqlFile);
	sqlStream.setCodec("UTF-8");
	foreach (Card card, m_cards) {
		QString index = card.uniqueFilename();
		if (!fileMap.contains(index)) {
			qDebug() << QString("Index '%1' not found for drawer %2").arg(index).arg(card.drawer());
			continue;
		}

		sqlStream << "#\n";
		sqlStream << QString("# %1,%2\n").arg(index).arg(fileMap[index]);
		sqlStream << "#\n";
		sqlStream << "# first fiche\n";
		sqlStream << QString::fromUtf8("insert into actual_entries (fiche, entry, comment) "
												 "values ('%1_0001', '%2', 'z opisu pudełka');\n").arg(index).arg(card.firstWord());
		sqlStream << QString::fromUtf8("insert into original_entries (fiche, entry, comment) "
												 "values ('%1_0001', '%2', 'z opisu pudełka');\n").arg(index).arg(card.firstWord());
		sqlStream << "# last fiche\n";
		QString baseFile = fileMap[index].section('.', -2);
		sqlStream << QString::fromUtf8("insert into actual_entries (fiche, entry, comment) "
												 "values ('%1', '%2', 'z opisu pudełka');\n").arg(baseFile).arg(card.lastWord());
		sqlStream << QString::fromUtf8("insert into original_entries (fiche, entry, comment) "
												 "values ('%1', '%2', 'z opisu pudełka');\n").arg(baseFile).arg(card.lastWord());

	}
	return true;
}
