#ifndef CARDS_H
#define CARDS_H

#include "card.h"
class Cards
{
public:
	Cards();
	void parse(const QString& filename);
	void sort();
	void setUniqueFiles();
	bool readCsv(const QString& csvFile);
	bool saveCsv(const QString& csvFile);
	bool saveMaleks(const QString& maleksFile);
	bool saveSql(const QString& djvuIndexFile, const QString& sqlFilename);
	QList<Card> m_cards;
};

#endif // CARDS_H
