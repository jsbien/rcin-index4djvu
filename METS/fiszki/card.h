#ifndef FISZKA_H
#define FISZKA_H

#include <QtCore>
#include <QtXml>

class Card
{
public:
	Card();
	void setFilename(const QString& filename) {m_filename = filename;}
	QString filename() const {return m_filename;}
	void setDrawer(const QString& drawer);
	QString drawer() const {return m_drawer;}
	QString firstWord() const {return m_firstWord;}
	QString lastWord() const {return m_lastWord;}
	int drawerId() const {return m_firstWordId;}
	void setSequence(int sequence) {m_sequence = sequence;}
	int sequence() const {return m_sequence;}
	void setUniqueFilename(const QString& unique) {m_uniqueFilename = unique;}
	QString uniqueFilename() const {return m_uniqueFilename;}
	QString errorMessage() const;
	bool parse(const QString& filename);
	/** Save card to semicolon-separated CSV row with EOL. */
	QString csvRow() const;
	/** Read the content from a line of CSV exported before. */
	bool parseCvsRow(const QString& row);
	bool operator<(const Card& card) const;
private:
	QDomElement firstElement(const QDomDocument &root, const QString& tag) const;

	QString m_filename;
	QString m_uniqueFilename;
	QString m_drawer;
	QString m_firstWord;
	QString m_lastWord;
	int m_firstWordId;
	int m_lastWordId;
	int m_sequence;
};

#endif // FISZKA_H
