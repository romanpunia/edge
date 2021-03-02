#ifndef TH_NETWORK_MONGODB_H
#define TH_NETWORK_MONGODB_H

#include "../core/network.h"

struct _bson_t;
struct _mongoc_client_t;
struct _mongoc_uri_t;
struct _mongoc_database_t;
struct _mongoc_collection_t;
struct _mongoc_cursor_t;
struct _mongoc_bulk_operation_t;
struct _mongoc_client_pool_t;
struct _mongoc_change_stream_t;
struct _mongoc_client_session_t;

#define TH_MDB_COMPILE(Args, Code) Tomahawk::Network::MDB::Driver::GetSubquery(#Code, Args, true)
#define TH_MDB_RECOMPILE(Args, Code) Tomahawk::Network::MDB::Driver::GetSubquery(#Code, Args, false)

namespace Tomahawk
{
	namespace Network
	{
		namespace MDB
		{
			typedef _bson_t TDocument;
			typedef _mongoc_client_pool_t TConnectionPool;
			typedef _mongoc_bulk_operation_t TStream;
			typedef _mongoc_cursor_t TCursor;
			typedef _mongoc_collection_t TCollection;
			typedef _mongoc_database_t TDatabase;
			typedef _mongoc_uri_t TAddress;
			typedef _mongoc_client_t TConnection;
			typedef _mongoc_change_stream_t TWatcher;
			typedef _mongoc_client_session_t TTransaction;
			typedef std::unordered_map<std::string, Rest::Document*> QueryMap;

			class Connection;

			class Queue;

			enum Query
			{
				Query_None = 0,
				Query_Tailable_Cursor = 1 << 1,
				Query_Slave_Ok = 1 << 2,
				Query_Oplog_Replay = 1 << 3,
				Query_No_Cursor_Timeout = 1 << 4,
				Query_Await_Data = 1 << 5,
				Query_Exhaust = 1 << 6,
				Query_Partial = 1 << 7,
			};

			enum Type
			{
				Type_Unknown,
				Type_Uncastable,
				Type_Null,
				Type_Document,
				Type_Array,
				Type_String,
				Type_Integer,
				Type_Number,
				Type_Boolean,
				Type_ObjectId,
				Type_Decimal
			};

			struct TH_OUT Property
			{
				std::string Name;
				std::string String;
				TDocument* Object;
				TDocument* Array;
				Type Mod;
				int64_t Integer;
				uint64_t High;
				uint64_t Low;
				double Number;
				unsigned char ObjectId[12] = { 0 };
				bool Boolean;
				bool IsValid;

				Property();
				~Property();
				void Release();
				std::string& ToString();
			};

			class TH_OUT Util
			{
			public:
				static bool GetId(unsigned char* Id12);
				static bool GetDecimal(const char* Value, int64_t* High, int64_t* Low);
				static unsigned int GetHashId(unsigned char* Id12);
				static int64_t GetTimeId(unsigned char* Id12);
				static std::string IdToString(unsigned char* Id12);
				static std::string StringToId(const std::string& Id24);
			};

			class TH_OUT Document
			{
			private:
				TDocument* Base;

			public:
				Document(TDocument* NewBase);
				void Release();
				void Join(const Document& Value);
				void Loop(const std::function<bool(Property*)>& Callback) const;
				bool SetDocument(const char* Key, Document* Value, uint64_t ArrayId = 0);
				bool SetArray(const char* Key, Document* Array, uint64_t ArrayId = 0);
				bool SetString(const char* Key, const char* Value, uint64_t ArrayId = 0);
				bool SetBlob(const char* Key, const char* Value, uint64_t Length, uint64_t ArrayId = 0);
				bool SetInteger(const char* Key, int64_t Value, uint64_t ArrayId = 0);
				bool SetNumber(const char* Key, double Value, uint64_t ArrayId = 0);
				bool SetDecimal(const char* Key, uint64_t High, uint64_t Low, uint64_t ArrayId = 0);
				bool SetDecimalString(const char* Key, const std::string& Value, uint64_t ArrayId = 0);
				bool SetDecimalInteger(const char* Key, int64_t Value, uint64_t ArrayId = 0);
				bool SetDecimalNumber(const char* Key, double Value, uint64_t ArrayId = 0);
				bool SetBoolean(const char* Key, bool Value, uint64_t ArrayId = 0);
				bool SetObjectId(const char* Key, unsigned char Value[12], uint64_t ArrayId = 0);
				bool SetNull(const char* Key, uint64_t ArrayId = 0);
				bool Set(const char* Key, Property* Value, uint64_t ArrayId = 0);
				bool Has(const char* Key) const;
				bool Get(const char* Key, Property* Output) const;
				bool Find(const char* Key, Property* Output) const;
				uint64_t Count() const;
				std::string ToRelaxedJSON() const;
				std::string ToExtendedJSON() const;
				std::string ToJSON() const;
				Rest::Document* ToDocument(bool IsArray = false) const;
				Document Copy() const;
				TDocument* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}

			public:
				static Document FromDocument(Rest::Document* Document);
				static Document FromEmpty(bool Array);
				static Document FromJSON(const std::string& JSON);
				static Document FromBuffer(const unsigned char* Buffer, uint64_t Length);

			private:
				static bool Clone(void* It, Property* Output);
			};

			class TH_OUT Address
			{
			private:
				TAddress* Base;

			public:
				Address(TAddress* NewBase);
				void Release();
				void SetOption(const char* Name, int64_t Value);
				void SetOption(const char* Name, bool Value);
				void SetOption(const char* Name, const char* Value);
				void SetAuthMechanism(const char* Value);
				void SetAuthSource(const char* Value);
				void SetCompressors(const char* Value);
				void SetDatabase(const char* Value);
				void SetUsername(const char* Value);
				void SetPassword(const char* Value);
				TAddress* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}

			public:
				static Address FromURI(const char* Value);
			};

			class TH_OUT Stream
			{
			private:
				TStream* Base;

			public:
				Stream(TStream* NewBase);
				void Release();
				bool RemoveMany(Document* Selector, Document* Options);
				bool RemoveOne(Document* Selector, Document* Options);
				bool ReplaceOne(Document* Selector, Document* Replacement, Document* Options);
				bool Insert(Document* Result, Document* Options);
				bool UpdateOne(Document* Selector, Document* Result, Document* Options);
				bool UpdateMany(Document* Selector, Document* Result, Document* Options);
				bool RemoveMany(const Document& Selector, const Document& Options);
				bool RemoveOne(const Document& Selector, const Document& Options);
				bool ReplaceOne(const Document& Selector, const Document& Replacement, const Document& Options);
				bool Insert(const Document& Result, const Document& Options);
				bool UpdateOne(const Document& Selector, const Document& Result, const Document& Options);
				bool UpdateMany(const Document& Selector, const Document& Result, const Document& Options);
				bool Execute(Document* Reply);
				bool Execute();
				uint64_t GetHint() const;
				TStream* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}

			public:
				static Stream FromEmpty(bool IsOrdered);
			};

			class TH_OUT Cursor
			{
			private:
				TCursor* Base;

			public:
				Cursor(TCursor* NewBase);
				void Release();
				void Receive(const std::function<bool(const Document&)>& Callback) const;
				void SetMaxAwaitTime(uint64_t MaxAwaitTime);
				void SetBatchSize(uint64_t BatchSize);
				bool SetLimit(int64_t Limit);
				bool SetHint(uint64_t Hint);
				bool Next() const;
				bool HasError() const;
				bool HasMoreData() const;
				int64_t GetId() const;
				int64_t GetLimit() const;
				uint64_t GetMaxAwaitTime() const;
				uint64_t GetBatchSize() const;
				uint64_t GetHint() const;
				Document GetCurrent() const;
				TCursor* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}
			};

			class TH_OUT Collection
			{
			private:
				TCollection* Base;

			public:
				Collection(TCollection* NewBase);
				void Release();
				bool UpdateMany(Document* Selector, Document* Update, Document* Options, Document* Reply);
				bool UpdateOne(Document* Selector, Document* Update, Document* Options, Document* Reply);
				bool Rename(const char* NewDatabaseName, const char* NewCollectionName);
				bool RenameWithOptions(const char* NewDatabaseName, const char* NewCollectionName, Document* Options);
				bool RenameWithRemove(const char* NewDatabaseName, const char* NewCollectionName);
				bool RenameWithOptionsAndRemove(const char* NewDatabaseName, const char* NewCollectionName, Document* Options);
				bool Remove(Document* Options);
				bool RemoveMany(Document* Selector, Document* Options, Document* Reply);
				bool RemoveOne(Document* Selector, Document* Options, Document* Reply);
				bool RemoveIndex(const char* Name, Document* Options);
				bool ReplaceOne(Document* Selector, Document* Replacement, Document* Options, Document* Reply);
				bool InsertMany(std::vector<Document>& List, Document* Options, Document* Reply);
				bool InsertOne(Document* Result, Document* Options, Document* Reply);
				bool FindAndModify(Document* Selector, Document* Sort, Document* Update, Document* Fields, Document* Reply, bool Remove, bool Upsert, bool New);
				uint64_t CountElementsInArray(Document* Match, Document* Filter, Document* Options) const;
				uint64_t CountDocuments(Document* Filter, Document* Options, Document* Reply) const;
				uint64_t CountDocumentsEstimated(Document* Options, Document* Reply) const;
				std::string StringifyKeyIndexes(Document* Keys) const;
				const char* GetName() const;
				Cursor FindIndexes(Document* Options) const;
				Cursor FindMany(Document* Filter, Document* Options) const;
				Cursor FindOne(Document* Filter, Document* Options) const;
				Cursor Aggregate(Query Flags, Document* Pipeline, Document* Options) const;
				Stream CreateStream(Document* Options);
				TCollection* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}
			};

			class TH_OUT Database
			{
			private:
				TDatabase* Base;

			public:
				Database(TDatabase* NewBase);
				void Release();
				bool HasCollection(const char* Name) const;
				bool RemoveAllUsers();
				bool RemoveUser(const char* Name);
				bool Remove();
				bool RemoveWithOptions(Document* Options);
				bool AddUser(const char* Username, const char* Password, Document* Roles, Document* Custom);
				std::vector<std::string> GetCollectionNames(Document* Options) const;
				const char* GetName() const;
				Cursor FindCollections(Document* Options) const;
				Collection CreateCollection(const char* Name, Document* Options);
				Collection GetCollection(const char* Name) const;
				TDatabase* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}
			};

			class TH_OUT Watcher
			{
			private:
				TWatcher * Base;

			public:
				Watcher(TWatcher* NewBase);
				void Release();
				bool Next(Document& Result) const;
				bool Error(Document& Result) const;
				TWatcher* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}

			public:
				static Watcher FromConnection(Connection* Connection, Document* Pipeline, Document* Options);
				static Watcher FromDatabase(Database& Src, Document* Pipeline, Document* Options);
				static Watcher FromCollection(Collection& Src, Document* Pipeline, Document* Options);
			};

			class TH_OUT Transaction
			{
			private:
				TTransaction* Base;

			public:
				Transaction(TTransaction* NewBase);
				void Release();
				bool Start();
				bool Abort();
				bool Commit(Document* Reply);
				TTransaction* Get() const;
				operator bool() const
				{
					return Base != nullptr;
				}

			public:
				static TTransaction* FromConnection(Connection* Client, Document& NewUid);
			};

			class TH_OUT Connection : public Rest::Object
			{
				friend Queue;
				friend Transaction;

			private:
				Transaction Session;
				TConnection* Base;
				Queue* Master;
				bool Connected;

			public:
				Connection();
				virtual ~Connection() override;
				void SetProfile(const char* Name);
				bool SetServer(bool Writeable);
				bool Connect(const std::string& Address);
				bool Connect(Address* URI);
				bool Disconnect();
				Cursor FindDatabases(Document* Options) const;
				Database GetDatabase(const char* Name) const;
				Database GetDefaultDatabase() const;
				Collection GetCollection(const char* DatabaseName, const char* Name) const;
				Address GetAddress() const;
				Queue* GetMaster() const;
				TConnection* Get() const;
				std::vector<std::string> GetDatabaseNames(Document* Options);
				bool IsConnected();
			};

			class TH_OUT Queue : public Rest::Object
			{
			private:
				TConnectionPool* Pool;
				Address SrcAddress;
				bool Connected;

			public:
				Queue();
				virtual ~Queue() override;
				void SetProfile(const char* Name);
				bool Connect(const std::string& Address);
				bool Connect(Address* URI);
				bool Disconnect();
				void Push(Connection** Client);
				Connection* Pop();
				TConnectionPool* Get() const;
				Address GetAddress() const;
			};

			class TH_OUT Driver
			{
			private:
				struct Sequence
				{
					std::string Request;
					Document Cache;

					Sequence();
				};

			private:
				static std::unordered_map<std::string, Sequence>* Queries;
				static std::mutex* Safe;
				static int State;

			public:
				static void Create();
				static void Release();
				static bool AddQuery(const std::string& Name, const char* Buffer, size_t Size);
				static bool AddDirectory(const std::string& Directory, const std::string& Origin = "");
				static bool RemoveQuery(const std::string& Name);
				static Document GetQuery(const std::string& Name, QueryMap* Map, bool Once = true);
				static Document GetSubquery(const char* Buffer, QueryMap* Map, bool Once = true);
				static std::vector<std::string> GetQueries();

			private:
				static std::string GetJSON(Rest::Document* Source);
			};
		}
	}
}
#endif