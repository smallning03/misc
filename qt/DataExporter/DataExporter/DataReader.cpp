#include "datareader.h"

#include <QDebug>
#include <QObject>
#include <QFile>
#include <QTextCodec>
#include <QDir>

#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE;

#include "DataStructs.h"

DataReader * DataReader::s_instance = nullptr;

DataReader * DataReader::instance()
{
	if (!s_instance)
	{
		s_instance = new DataReader;
	}
	return s_instance;
}

DataReader::DataReader()
{

}

DataReader::~DataReader()
{

}

DataFile * DataReader::loadData( const QString &filename )
{
	Document doc(filename);

	DataFile *dataFile = nullptr;

	// ��������sheet;
	auto names = doc.sheetNames();
	for (auto it=names.begin(); it!=names.end(); ++it)
	{
		// ��ȡsheet;
		const auto &sheetName = *it;
		doc.selectSheet(sheetName);
		Worksheet *sheet = doc.currentWorksheet();

		// ���з�Χ;
		auto range = sheet->dimension();

		DataSheet *dataSheet = nullptr;

		// ��������cell;
		for (int row=range.firstRow(); row<=range.lastRow(); ++row)
		{
			for (int col=range.firstColumn(); col<=range.lastColumn(); ++col)
			{
				auto cell = sheet->cellAt(row, col);
				if (!cell) continue;

				// ����ͷ;
				auto &val = cell->value();
				if (val.type() == QVariant::String)
				{
					const auto &s = val.toString();
					if (sheetName.compare("LOCALE", Qt::CaseSensitivity::CaseInsensitive) == 0 &&
						s.compare("LOCALE", Qt::CaseSensitivity::CaseInsensitive) == 0)
					{
						auto table = this->parseLocale(sheet, row, col);
						if (table)
						{
							if (!dataSheet)
							{
								dataSheet = new DataSheet(sheetName);
							}

							dataSheet->addTable(table);
						}	
					}
					else if (s.length() >=2 && s[0] == QChar('#') && s[1] == QChar('^'))
					{
						// ��ͨ���ݱ�;
						qDebug() << QObject::tr("find table at ") << row << QObject::tr(", ") << col;
						auto table = this->parseTable(sheet, row, col);
						if (table)
						{
							if (!dataSheet)
							{
								dataSheet = new DataSheet(sheetName);
							}

							dataSheet->addTable(table);
						}
					}
				}
			}
		}

		// �����sheet���б��Ļ�;
		if (dataSheet)
		{
			if (!dataFile)
			{
				dataFile = new DataFile(filename);
			}

			dataFile->addSheet(dataSheet);
		}
	}

	return dataFile;
}

DataTable *DataReader::parseTable( Worksheet *sheet, int tableRow, int tableCol )
{
	// ��������;
	auto tableCell = sheet->cellAt(tableRow, tableCol);
	if (!tableCell) 
	{
		return nullptr;
	}
	auto tableName = tableCell->value().toString();
	tableName = tableName.mid(2);

	auto dataTable = new DataTable(tableName);

	// ��Ч��;
	QList<int> cols;

	// ����;
	QList<int> keys;

	// ��Ч����;
	QStringList names;

	// ���з�Χ;
	auto range = sheet->dimension();

	// ������ͷ;
	for (int col=tableCol+1; col<=range.lastColumn(); ++col)
	{
		auto cell = sheet->cellAt(tableRow, col);
		if (!cell) continue;

		auto &val = cell->value();
		if (val.type() == QVariant::String)
		{
			const auto &s = val.toString();

			// ����Ƿ����;
			if (s.length() >=2 && s[0] == QChar('#') && s[1] == QChar('$'))
			{
				break;
			}
			// ����Ƿ�Ϊ��Ч��;
			else if (s.length() >= 1 && s[0] == QChar('#'))
			{
				auto colIndex = cols.size();
				cols.append(col);

				// ��������;
				if (s.length() >= 2 && s[1] == QChar('*'))
				{
					auto &colName = s.mid(2);
					names.push_back(colName);
					keys.push_back(colIndex);
				}
				else
				{
					names.push_back(s.mid(1));
				}

			}
		}
	}

	// ���ñ�ͷ;
	dataTable->setHeader(names);
	dataTable->setKeys(keys);

	// ��������¼;
	for (int row=tableRow+1; row<=range.lastRow(); ++row)
	{
		auto cell = sheet->cellAt(row, tableCol);
		if (!cell) continue;

		auto &val = cell->value();
		if (val.type() == QVariant::String)
		{
			const auto &s = val.toString();

			// ����Ƿ����;
			if (s.length() >=2 && s[0] == QChar('#') && s[1] == QChar('$'))
			{
				break;
			}
			// ����Ƿ�Ϊ��Ч��;
			else if (s.length() >= 1 && s[0] == QChar('#'))
			{
				// ������Ч�н�������;
				QVariantList vars;
				for (auto it=cols.begin(); it!=cols.end(); ++it)
				{
					auto col = *it;

					auto dataCell = sheet->cellAt(row, col);
					if (!dataCell) 
					{
						// ����Ϊ�գ�����Ĭ��ֵ;
						vars.push_back(QVariant());
					}
					else
					{
						vars.push_back(dataCell->value());
					}
				}

				// �������ݱ���;
				dataTable->addRecord(vars);
			}
		}
	}

	return dataTable;
}

DataTable *DataReader::parseLocale( QXlsx::Worksheet *sheet, int tableRow, int tableCol )
{
	auto dataTable = new DataTable("LOCALE");

	// ��Ч��;
	QList<int> cols;

	// ��Ч����;
	QStringList names;

	// ���з�Χ;
	auto range = sheet->dimension();

	// ������ͷ;
	for (int col=tableCol; col<=range.lastColumn(); ++col)
	{
		auto cell = sheet->cellAt(tableRow, col);
		if (!cell) continue;

		auto &val = cell->value();
		if (val.type() == QVariant::String)
		{
			const auto &s = val.toString();

			// ��Ч��;
			if (!s.isEmpty())
			{
				// ��������;
				auto &colName = s;
				names.push_back(colName);
				cols.push_back(col);
			}
		}
	}

	// ���ñ�ͷ;
	dataTable->setHeader(names);

	// ��������¼;
	for (int row=tableRow+1; row<=range.lastRow(); ++row)
	{
		auto cell = sheet->cellAt(row, tableCol);
		if (!cell) continue;

		auto &val = cell->value();
		if (val.type() == QVariant::String)
		{
			const auto &s = val.toString();

			// ��Ч��;
			if (!s.isEmpty())
			{
				// ������Ч�н�������;
				QVariantList vars;
				for (auto it=cols.begin(); it!=cols.end(); ++it)
				{
					auto col = *it;

					auto dataCell = sheet->cellAt(row, col);
					if (!dataCell) 
					{
						// ����Ϊ�գ�����Ĭ��ֵ;
						vars.push_back(QString());
					}
					else
					{
						vars.push_back(dataCell->value().toString());
					}
				}

				// �������ݱ���;
				dataTable->addRecord(vars);
			}
		}
	}

	return dataTable;
}

bool DataReader::exportLua( DataTable *dataTable, const QString &path )
{
	const QString &name = dataTable->getName();
	const QStringList &header = dataTable->getHeader();
	const QList<int> &keys = dataTable->getKeys();
	const QList<QVariantList> &records = dataTable->getRecords();

	// ����Ŀ¼;
	QString dir_path = path + QObject::tr("/lua");
	QDir dir;
	bool exist = dir.exists(dir_path);
	if (!exist)
	{
		bool ok = dir.mkdir(dir_path);
		if (!ok)
		{
			return false;
		}
	}

	auto filename = dir_path + QObject::tr("/") + name.toLower() + QObject::tr(".lua");

	QString content;

	// luaͷ;
	content.append("-- WARNNING:\n");
	content.append("-- This lua file is generated by DataExporter.\n");
	content.append("-- Please don't modify manually.\n\n");
	content.append("return\n{\n");

	// ���б�;
	content.append("    _keys =\n    {\n");
	for (auto it=keys.begin(); it!=keys.end(); ++it)
	{
		auto keyIndex = *it;
		const auto &keyName = header[keyIndex];

		content.append("        \"");
		content.append(keyName);
		content.append("\",\n");
	}
	content.append("    },\n\n");

	// ӳ���;
	content.append("    _map =\n    {\n");
	if (keys.size() == 0)
	{

	}
	else if (keys.size() == 1)
	{
		auto keyIndex = keys[0];

		// ������map��;
		QMap<QString, int> map;
		int dataIndex = 1;
		for (auto it=records.begin(); it!=records.end(); ++it, ++dataIndex)
		{
			const auto &record = *it;
			auto key = record[keyIndex].toString();
			map[key] = dataIndex;
		}

		// �������;
		for (auto it=map.begin(); it!=map.end(); ++it)
		{
			const auto &key = it.key();
			auto dataIndex = it.value();
			content.append("        [\"");
			content.append(key);
			content.append("\"] = ");
			content.append(QString::number(dataIndex));
			content.append(",\n");
		}
	}
	else if (keys.size() == 2)
	{
		auto keyIndex1 = keys[0];
		auto keyIndex2 = keys[1];

		// ������map��;
		QMap<QString, QMap<QString, int> > map;
		int dataIndex = 1;
		for (auto it=records.begin(); it!=records.end(); ++it, ++dataIndex)
		{
			const auto &record = *it;
			auto key1 = record[keyIndex1].toString();
			auto key2 = record[keyIndex2].toString();

			auto itFind = map.find(key1);
			if (itFind == map.end())
			{
				map[key1] = QMap<QString, int>();
			}

			map[key1][key2] = dataIndex;
		}

		// �������;
		for (auto it=map.begin(); it!=map.end(); ++it)
		{
			const auto &key1 = it.key();
			const auto &map1 = it.value();

			content.append("        [\"");
			content.append(key1);
			content.append("\"] = {\n");
			for (auto it2=map1.begin(); it2!=map1.end(); ++it2)
			{
				const auto &key2 = it2.key();
				auto dataIndex = it2.value();
				content.append("            [\"");
				content.append(key2);
				content.append("\"] = ");
				content.append(QString::number(dataIndex));
				content.append(",\n");
			}
			content.append("        },\n");
		}
	}
	else if (keys.size() == 3)
	{
		auto keyIndex1 = keys[0];
		auto keyIndex2 = keys[1];
		auto keyIndex3 = keys[2];

		// ������map��;
		QMap<QString, QMap<QString, QMap<QString, int> > > map;
		int dataIndex = 1;
		for (auto it=records.begin(); it!=records.end(); ++it, ++dataIndex)
		{
			const auto &record = *it;
			auto key1 = record[keyIndex1].toString();
			auto key2 = record[keyIndex2].toString();
			auto key3 = record[keyIndex3].toString();

			auto itFind = map.find(key1);
			if (itFind == map.end())
			{
				map[key1] = QMap<QString, QMap<QString, int> >();
			}

			auto itFind2 = map[key1].find(key2);
			if (itFind2 == map[key1].end())
			{
				map[key1][key2] = QMap<QString, int>();
			}

			map[key1][key2][key3] = dataIndex;
		}

		// �������;
		for (auto it=map.begin(); it!=map.end(); ++it)
		{
			const auto &key1 = it.key();
			const auto &map1 = it.value();

			content.append("        [\"");
			content.append(key1);
			content.append("\"] = {\n");
			for (auto it2=map1.begin(); it2!=map1.end(); ++it2)
			{
				const auto &key2 = it2.key();
				const auto &map2 = it2.value();
				content.append("            [\"");
				content.append(key2);
				content.append("\"] = {\n");
				for (auto it3=map2.begin(); it3!=map2.end(); ++it3)
				{
					const auto &key3 = it3.key();
					auto dataIndex = it3.value();
					content.append("                [\"");
					content.append(key3);
					content.append("\"] = ");
					content.append(QString::number(dataIndex));
					content.append(",\n");
				}
				content.append("            },\n");
			}
			content.append("        },\n");
		}
	}
	else
	{
		// Ŀǰֻ֧�����3��key;
		Q_ASSERT(false);
	}
	content.append("    },\n\n");


	// ����;
	content.append("    _data = \n    {\n");
	for (auto it=records.begin(); it!=records.end(); ++it)
	{
		content.append("        {\n");
		const auto &record = *it;
		auto itHeader = header.begin();
		auto itRecord = record.begin();
		for ( ; itHeader!=header.end(); ++itHeader, ++itRecord)
		{
			const auto &colName = *itHeader;
			const auto &colData = *itRecord;

			bool isstring = false;
			if (colData.type() == QVariant::String)
			{
				isstring = true;
			}

			content.append("            [\"");
			content.append(colName);
			content.append("\"] = ");
			if (isstring) content.append("\"");
			content.append(colData.toString());
			if (isstring) content.append("\"");
			content.append(",\n");
		}
		content.append("        },\n");
	}
	content.append("    }\n");

	// lua�ļ�β;
	content.append("}\n");

	// ����ļ�;
	QFile f(filename);
	if (f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream output(&f);
		output.setCodec("utf-8");
		output << content << endl;
		f.close();
	}

	return true;
}

bool DataReader::exportXml( DataTable *dataTable, const QString &path )
{
	const QString &name = dataTable->getName();
	const QStringList &header = dataTable->getHeader();
	const QList<QVariantList> &records = dataTable->getRecords();

	// ����Ŀ¼;
	QString dir_path = path + QObject::tr("/xml");
	QDir dir;
	bool exist = dir.exists(dir_path);
	if (!exist)
	{
		bool ok = dir.mkdir(dir_path);
		if (!ok)
		{
			return false;
		}
	}

	auto filename = dir_path + QObject::tr("/") + name.toLower() + QObject::tr(".xml");

	QString content;

	// xmlͷ;
	content.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	content.append("<");
	content.append(name);
	content.append(" No=\"\" Name=\"\">\n");

	// ����;
	for (auto it=records.begin(); it!=records.end(); ++it)
	{
		content.append("  <Property ");
		const auto &record = *it;
		auto itHeader = header.begin();
		auto itRecord = record.begin();
		for ( ; itHeader!=header.end(); ++itHeader, ++itRecord)
		{
			const auto &colName = *itHeader;
			const auto &colData = *itRecord;

			content.append(colName);
			content.append("=\"");
			content.append(colData.toString());
			content.append("\" ");
		}
		content.append("/>\n");
	}

	content.append("</");
	content.append(name);
	content.append(">\n");

	// ����ļ�;
	QFile f(filename);
	if (f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream output(&f);
		output.setCodec("utf-8");
		output << content << endl;
		f.close();
	}

	return true;
}

bool DataReader::exportLocale( DataTable *dataTable, const QString &path )
{
	const QString &name = dataTable->getName();
	const QStringList &header = dataTable->getHeader();
	const QList<QVariantList> &records = dataTable->getRecords();

	auto locale_path = path + QObject::tr("/locale");

	// ����localeĿ¼;
	QDir locale_dir;
	bool exist = locale_dir.exists(locale_path);
	if (!exist)
	{
		bool ok = locale_dir.mkdir(locale_path);
		if (!ok)
		{
			return false;
		}
	}

	QStringList origins;

	// ������ͬ����;
	int col = 0;
	for (auto it=header.begin(); it!=header.end(); ++it, ++col) 
	{
		const auto &lang = *it;
		if (lang.isEmpty()) continue;

		// ����ԭ�ַ���;
		if (lang.compare("LOCALE", Qt::CaseSensitivity::CaseInsensitive) == 0) 
		{
			for (auto it=records.begin(); it!=records.end(); ++it)
			{
				const auto &val = *it;
				const auto &s = val[col].toString();
				origins.push_back(s);
			}
			continue;
		}

		auto lang_path = locale_path + QObject::tr("/") + lang;

		// ��������Ŀ¼;
		QDir lang_dir;
		bool exist = lang_dir.exists(lang_path);
		if (!exist)
		{
			bool ok = lang_dir.mkdir(lang_path);
			if (!ok)
			{
				return false;
			}
		}

		QString content;

		// jsonͷ;
		content.append("{\n");

		// ���������ַ���;
		int row = 0;
		for (auto it=records.begin(); it!=records.end(); ++it, ++row)
		{
			const auto &record = *it;
			const auto &s = record[col].toString();

			content.append("    \"");
			content.append(origins[row]);
			content.append("\" : \"");
			content.append(s);
			content.append("\",\n");
		}

		// jsonβ;
		content.append("    \"\" : \"\"\n}\n");

		// ����ļ�;
		auto filename = lang_path + QObject::tr("/text.json");
		QFile f(filename);
		if (f.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream output(&f);
			output.setCodec("utf-8");
			output << content << endl;
			f.close();
		}
		else
		{
			return false;
		}

	}

	return true;
}

bool DataReader::exportData( DataTable *dataTable, const QString &path )
{
	if (dataTable->getName().compare("LOCALE", Qt::CaseSensitivity::CaseInsensitive) == 0)
	{
		this->exportLocale(dataTable, path);
	}
	else
	{
		this->exportLua(dataTable, path);
		this->exportXml(dataTable, path);
	}

	return true;
}
