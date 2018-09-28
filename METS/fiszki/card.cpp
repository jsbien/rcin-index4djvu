#include "card.h"

Card::Card()
{
	m_firstWordId = 1;
	m_lastWordId = 1;
	m_sequence = 0;
}

void Card::setDrawer(const QString &drawer)
{
	QLocale locale =  QLocale::system();
	m_drawer = drawer;
	if (m_drawer.contains(" - ")) {
		m_firstWord = m_drawer.section(" - ", 0, 0).trimmed();
		m_lastWord = m_drawer.section(" - ", 1, 1).trimmed();
	}
	else if (m_drawer.contains('-')) {
		m_firstWord = m_drawer.section('-', 0, 0).trimmed();
		m_lastWord = m_drawer.section('-', 1, 1).trimmed();
	}
	else m_firstWord = m_lastWord = m_drawer;

	m_firstWord = locale.toLower(m_firstWord);
	m_firstWord.replace('_', ' ');
	m_lastWord = locale.toLower(m_lastWord);
	m_lastWord.replace('_', ' ');

	QRegExp digits("\\d+");
	if (m_firstWord.contains(digits)) {
		m_firstWordId = digits.cap().toInt();
		m_firstWord.remove(digits.cap());
	}
	if (m_lastWord.contains(digits)) {
		m_lastWordId = digits.cap().toInt();
		m_lastWord.remove(digits.cap());
	}

	m_firstWord.remove("(c.d.)");
	m_firstWord.remove("-cd");
	m_lastWord.remove("(c.d.)");
	m_lastWord.remove("-cd");
}

QString Card::errorMessage() const
{
	if (m_firstWord.localeAwareCompare(m_lastWord) > 0)
		return "Bad order";
	else return "";
}

bool Card::parse(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDomDocument document;
	if (!document.setContent(&file))
		return false;

	QDomElement fileElement = firstElement(document, "FLocat");
	if (!fileElement.isNull())
		setFilename(fileElement.attribute("xlink:href").section('/', -1, -1));

	QDomElement drawerElement = firstElement(document, "dc:title");
	if (!drawerElement.isNull()) {
		QString drawer = drawerElement.text().section(";", 1).trimmed();
		drawer.replace(';', ',');
		setDrawer(drawer);
	}
	return true;
}

QString Card::csvRow() const
{
	QStringList fields;
	fields.append(m_filename);
	fields.append(m_drawer);
	fields.append(uniqueFilename());
	fields.append(m_firstWord);
	fields.append(m_lastWord);
	fields.append(QString::number(m_firstWordId));
	fields.append(QString::number(m_lastWordId));
	fields.append(QString::number(m_sequence));
	fields.append(errorMessage());
	return fields.join(";") + "\n";
}

bool Card::parseCvsRow(const QString &row)
{
	QStringList fields = row.split(';');
	if (fields.count() < 9)
		return false;
	m_filename = fields[0];
	m_drawer = fields[1];
	m_uniqueFilename = fields[2];
	m_firstWord = fields[3];
	m_lastWord = fields[4];
	m_firstWordId = qMax(fields[5].toInt(), 1);
	m_lastWordId = qMax(fields[6].toInt(), 1);
	m_sequence = fields[7].toInt();
	// fields[8] is error message
	return true;
}

bool Card::operator <(const Card &card) const
{
	int cmp = QString::localeAwareCompare(m_firstWord, card.m_firstWord);
	return cmp < 0  || (cmp == 0 && m_firstWordId < card.m_firstWordId);
}

QDomElement Card::firstElement(const QDomDocument &document, const QString &tag) const
{
	QDomNodeList list = document.elementsByTagName(tag);
	if (list.isEmpty())
		return QDomElement();
	else return list.at(0).toElement();
}
