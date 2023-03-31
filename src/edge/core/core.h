#ifndef ED_CORE_H
#define ED_CORE_H
#pragma warning(disable: 4251)
#if defined(_WIN32) || defined(_WIN64)
#define ED_MICROSOFT 1
#define ED_CDECL __cdecl
#define ED_FILENO _fileno
#define ED_PATH_SPLIT '\\'
#define ED_MAX_PATH MAX_PATH
#ifndef ED_EXPORT
#define ED_OUT __declspec(dllimport)
#else
#define ED_OUT __declspec(dllexport)
#endif
#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L || defined(_HAS_CXX17)
#define ED_FAST_SORT
#endif
#ifdef _WIN64
#define ED_64 1
#else
#define ED_32 1
#endif
#elif defined(__APPLE__)
#define _XOPEN_SOURCE
#define ED_APPLE 1
#define ED_UNIX 1
#define ED_CDECL
#define ED_OUT
#define ED_FILENO fileno
#define ED_PATH_SPLIT '/'
#ifdef PATH_MAX
#define ED_MAX_PATH PATH_MAX
#else
#define ED_MAX_PATH _POSIX_PATH_MAX
#endif
#if __x86_64__ || __ppc64__
#define ED_64 1
#else
#define ED_32 1
#endif
#elif defined __linux__ && defined __GNUC__
#define ED_UNIX 1
#define ED_CDECL
#define ED_OUT
#define ED_FILENO fileno
#define ED_PATH_SPLIT '/'
#ifdef PATH_MAX
#define ED_MAX_PATH PATH_MAX
#else
#define ED_MAX_PATH _POSIX_PATH_MAX
#endif
#if __x86_64__ || __ppc64__
#define ED_64 1
#else
#define ED_32 1
#endif
#endif
#include <inttypes.h>
#include <thread>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <cstdarg>
#include <cstring>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <condition_variable>
#include <atomic>
#include <limits>
#include <array>
#ifdef ED_FAST_SORT
#include <execution>
#define ED_SORT(Begin, End, Comparator) std::sort(std::execution::par_unseq, Begin, End, Comparator)
#else
#define ED_SORT(Begin, End, Comparator) std::sort(Begin, End, Comparator)
#endif
#ifdef ED_MICROSOFT
#ifdef ED_USE_FCTX
#define ED_COCALL
#define ED_CODATA void* Context
#else
#define ED_COCALL __stdcall
#define ED_CODATA void* Context
#endif
typedef size_t socket_t;
typedef int socket_size_t;
typedef void* epoll_handle;
#else
#ifdef ED_USE_FCTX
#define ED_COCALL
#define ED_CODATA void* Context
#else
#define ED_COCALL
#define ED_CODATA int X, int Y
#endif
#include <sys/types.h>
#include <sys/socket.h>
typedef int epoll_handle;
typedef int socket_t;
typedef socklen_t socket_size_t;
#endif
#ifdef NDEBUG
#if ED_DLEVEL >= 5
#define ED_TRACE(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Trace, 0, nullptr, Format, ##__VA_ARGS__)
#else
#define ED_TRACE(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 4
#define ED_DEBUG(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Debug, 0, nullptr, Format, ##__VA_ARGS__)
#else
#define ED_DEBUG(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 3
#define ED_INFO(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Info, 0, nullptr, Format, ##__VA_ARGS__)
#else
#define ED_INFO(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 2
#define ED_WARN(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Warning, 0, nullptr, Format, ##__VA_ARGS__)
#else
#define ED_WARN(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 1
#define ED_ERR(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Error, 0, nullptr, Format, ##__VA_ARGS__)
#else
#define ED_ERR(Format, ...) ((void)0)
#endif
#define ED_ASSERT(Condition, Returnable, Format, ...) ((void)0)
#define ED_ASSERT_V(Condition, Format, ...) ((void)0)
#define ED_MEASURE(Threshold) ((void)0)
#define ED_MEASURE_LOOP() ((void)0)
#define ED_AWAIT(Value) Edge::Core::Coawait(Value)
#define ED_WATCH(Ptr, Label) ((void)0)
#define ED_WATCH_AT(Ptr, Function, Label) ((void)0)
#define ED_UNWATCH(Ptr) ((void)0)
#define ED_MALLOC(Type, Size) (Type*)Edge::Core::OS::Malloc(Size)
#define ED_NEW(Type, ...) new((void*)ED_MALLOC(Type, sizeof(Type))) Type(__VA_ARGS__)
#else
#if ED_DLEVEL >= 5
#define ED_TRACE(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Trace, __LINE__, __FILE__, Format, ##__VA_ARGS__)
#else
#define ED_TRACE(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 4
#define ED_DEBUG(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Debug, __LINE__, __FILE__, Format, ##__VA_ARGS__)
#else
#define ED_DEBUG(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 3
#define ED_INFO(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Info, __LINE__, __FILE__, Format, ##__VA_ARGS__)
#else
#define ED_INFO(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 2
#define ED_WARN(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Warning, __LINE__, __FILE__, Format, ##__VA_ARGS__)
#else
#define ED_WARN(Format, ...) ((void)0)
#endif
#if ED_DLEVEL >= 1
#define ED_ERR(Format, ...) Edge::Core::OS::Log((int)Edge::Core::LogLevel::Error, __LINE__, __FILE__, Format, ##__VA_ARGS__)
#define ED_ASSERT(Condition, Returnable, Format, ...) if (!(Condition)) { Edge::Core::OS::Assert(true, __LINE__, __FILE__, __func__, #Condition, Format, ##__VA_ARGS__); return Returnable; }
#define ED_ASSERT_V(Condition, Format, ...) if (!(Condition)) { Edge::Core::OS::Assert(true, __LINE__, __FILE__, __func__, #Condition, Format, ##__VA_ARGS__); return; }
#else
#define ED_ERR(Format, ...) ((void)0)
#define ED_ASSERT(Condition, Returnable, Format, ...) if (!(Condition)) { Edge::Core::OS::Process::Interrupt(); return Returnable; }
#define ED_ASSERT_V(Condition, Format, ...) if (!(Condition)) { Edge::Core::OS::Process::Interrupt(); return; }
#endif
#define ED_MEASURE_START(X) _measure_line_##X
#define ED_MEASURE_PREPARE(X) ED_MEASURE_START(X)
#define ED_MEASURE(Threshold) auto ED_MEASURE_PREPARE(__LINE__) = Edge::Core::OS::Timing::Measure(__FILE__, __func__, __LINE__, Threshold)
#define ED_MEASURE_LOOP() Edge::Core::OS::Timing::MeasureLoop()
#define ED_AWAIT(Value) Edge::Core::Coawait(Value, __func__, #Value)
#define ED_WATCH(Ptr, Label) Edge::Core::OS::Watch(Ptr, Edge::Core::Allocator::Context(__FILE__, __func__, Label, __LINE__))
#define ED_WATCH_AT(Ptr, Function, Label) Edge::Core::OS::Watch(Ptr, Edge::Core::Allocator::Context(__FILE__, Function, Label, __LINE__))
#define ED_UNWATCH(Ptr) Edge::Core::OS::Unwatch(Ptr)
#define ED_MALLOC(Type, Size) (Type*)Edge::Core::OS::MallocDebug(Size, Edge::Core::Allocator::Context(__FILE__, __func__, typeid(Type).name(), __LINE__))
#define ED_NEW(Type, ...) new((void*)ED_MALLOC(Type, sizeof(Type))) Type(__VA_ARGS__)
#endif
#ifdef max
#undef max
#endif
#define ED_DELETE(Destructor, Var) { if (Var != nullptr) { (Var)->~Destructor(); ED_FREE((void*)Var); } }
#define ED_FREE(Ptr) Edge::Core::OS::Free(Ptr)
#define ED_RELEASE(Ptr) { if (Ptr != nullptr) (Ptr)->Release(); }
#define ED_CLEAR(Ptr) { if (Ptr != nullptr) { (Ptr)->Release(); Ptr = nullptr; } }
#define ED_ASSIGN(FromPtr, ToPtr) { (FromPtr) = ToPtr; if (FromPtr != nullptr) (FromPtr)->AddRef(); }
#define ED_REASSIGN(FromPtr, ToPtr) { ED_RELEASE(FromPtr); (FromPtr) = ToPtr; if (FromPtr != nullptr) (FromPtr)->AddRef(); }
#define ED_HASH(Name) Edge::Core::OS::File::GetCheckSum(Name)
#define ED_STRINGIFY(Text) #Text
#define ED_TIMING_ATOM (1)
#define ED_TIMING_FRAME (5)
#define ED_TIMING_CORE (16)
#define ED_TIMING_MIX (50)
#define ED_TIMING_IO (80)
#define ED_TIMING_NET (150)
#define ED_TIMING_MAX (350)
#define ED_TIMING_HANG (5000)
#define ED_TIMING_INFINITE (0)
#define ED_STACK_SIZE (512 * 1024)
#define ED_CHUNK_SIZE (2048)
#define ED_BIG_CHUNK_SIZE (8192)
#define ED_MAX_EVENTS (32)
#define ED_INVALID_TASK_ID (0)
#define ED_OUT_TS ED_OUT

namespace Edge
{
	namespace Core
	{
		struct ConcurrentQueuePtr;

		struct Decimal;

		struct Cocontext;

		class Console;

		class Costate;

		class Schema;

		class Stream;

		class Var;

		enum class Deferred : uint8_t
		{
			None = 0,
			Pending = 1,
			Ready = 2
		};

		enum class StdColor
		{
			Black = 0,
			DarkBlue = 1,
			DarkGreen = 2,
			LightBlue = 3,
			DarkRed = 4,
			Magenta = 5,
			Orange = 6,
			LightGray = 7,
			Gray = 8,
			Blue = 9,
			Green = 10,
			Cyan = 11,
			Red = 12,
			Pink = 13,
			Yellow = 14,
			White = 15,
			Zero = 16
		};

		enum class FileMode
		{
			Read_Only,
			Write_Only,
			Append_Only,
			Read_Write,
			Write_Read,
			Read_Append_Write,
			Binary_Read_Only,
			Binary_Write_Only,
			Binary_Append_Only,
			Binary_Read_Write,
			Binary_Write_Read,
			Binary_Read_Append_Write
		};

		enum class FileSeek
		{
			Begin,
			Current,
			End
		};

		enum class VarType : uint8_t
		{
			Null,
			Undefined,
			Object,
			Array,
			Pointer,
			String,
			Binary,
			Integer,
			Number,
			Decimal,
			Boolean
		};

		enum class VarForm
		{
			Dummy,
			Tab_Decrease,
			Tab_Increase,
			Write_Space,
			Write_Line,
			Write_Tab,
		};

		enum class Coactive
		{
			Active,
			Inactive,
			Resumable
		};

		enum class Difficulty
		{
			Coroutine,
			Light,
			Heavy,
			Clock,
			Count
		};

		enum class LogLevel
		{
			Error = 1,
			Warning = 2,
			Info = 3,
			Debug = 4,
			Trace = 5
		};

		template <typename T>
		using Unique = T*;

		typedef std::vector<struct Variant> VariantList;
		typedef std::vector<Schema*> SchemaList;
		typedef std::unordered_map<std::string, struct Variant> VariantArgs;
		typedef std::unordered_map<std::string, Schema*> SchemaArgs;
		typedef std::function<void()> TaskCallback;
		typedef std::function<void(size_t)> SeqTaskCallback;
		typedef std::function<std::string(const std::string&)> SchemaNameCallback;
		typedef std::function<void(VarForm, const char*, size_t)> SchemaWriteCallback;
		typedef std::function<bool(char*, size_t)> SchemaReadCallback;
		typedef std::function<bool()> ActivityCallback;
		typedef uint64_t TaskId;
		typedef Decimal BigNumber;

		template <typename Type>
		struct Mapping
		{
			Type Map;
		};

		struct ED_OUT Coroutine
		{
			friend Costate;

		private:
			std::atomic<Coactive> State;
			std::atomic<int> Dead;
			TaskCallback Callback;
			Cocontext* Slave;
			Costate* Master;

		public:
			TaskCallback Return;

		private:
			Coroutine(Costate* Base, const TaskCallback& Procedure) noexcept;
			Coroutine(Costate* Base, TaskCallback&& Procedure) noexcept;
			~Coroutine() noexcept;
		};

		struct ED_OUT Decimal
		{
		private:
			std::deque<char> Source;
			int Length;
			char Sign;
			bool Invalid;

		public:
			Decimal() noexcept;
			Decimal(const char* Value) noexcept;
			Decimal(const std::string& Value) noexcept;
			Decimal(int32_t Value) noexcept;
			Decimal(uint32_t Value) noexcept;
			Decimal(int64_t Value) noexcept;
			Decimal(uint64_t Value) noexcept;
			Decimal(float Value) noexcept;
			Decimal(double Value) noexcept;
			Decimal(const Decimal& Value) noexcept;
			Decimal(Decimal&& Value) noexcept;
			Decimal& Truncate(int Value);
			Decimal& Round(int Value);
			Decimal& Trim();
			Decimal& Unlead();
			Decimal& Untrail();
			bool IsNaN() const;
			bool IsZero() const;
			bool IsZeroOrNaN() const;
			bool IsPositive() const;
			bool IsNegative() const;
			double ToDouble() const;
			float ToFloat() const;
			int64_t ToInt64() const;
			uint64_t ToUInt64() const;
			std::string ToString() const;
			std::string Exp() const;
			int Decimals() const;
			int Ints() const;
			int Size() const;
			Decimal operator -() const;
			Decimal& operator *=(const Decimal& V);
			Decimal& operator /=(const Decimal& V);
			Decimal& operator +=(const Decimal& V);
			Decimal& operator -=(const Decimal& V);
			Decimal& operator= (const Decimal& Value) noexcept;
			Decimal& operator= (Decimal&& Value) noexcept;
			Decimal& operator++ (int);
			Decimal& operator++ ();
			Decimal& operator-- (int);
			Decimal& operator-- ();
			bool operator== (const Decimal& Right) const;
			bool operator!= (const Decimal& Right) const;
			bool operator> (const Decimal& Right) const;
			bool operator>= (const Decimal& Right) const;
			bool operator< (const Decimal& Right) const;
			bool operator<= (const Decimal& Right) const;
			explicit operator double () const
			{
				return ToDouble();
			}
			explicit operator float() const
			{
				return ToFloat();
			}
			explicit operator int64_t() const
			{
				return ToInt64();
			}
			explicit operator uint64_t() const
			{
				return ToUInt64();
			}
			explicit operator std::string() const
			{
				return ToString();
			}

		public:
			ED_OUT friend Decimal operator + (const Decimal& Left, const Decimal& Right);
			ED_OUT friend Decimal operator - (const Decimal& Left, const Decimal& Right);
			ED_OUT friend Decimal operator * (const Decimal& Left, const Decimal& Right);
			ED_OUT friend Decimal operator / (const Decimal& Left, const Decimal& Right);
			ED_OUT friend Decimal operator % (const Decimal& Left, const Decimal& Right);

		public:
			static Decimal Divide(const Decimal& Left, const Decimal& Right, int Precision);
			static Decimal NaN();

		private:
			static Decimal Sum(const Decimal& Left, const Decimal& Right);
			static Decimal Subtract(const Decimal& Left, const Decimal& Right);
			static Decimal Multiply(const Decimal& Left, const Decimal& Right);
			static int CompareNum(const Decimal& Left, const Decimal& Right);
			static int CharToInt(const char& Value);
			static char IntToChar(const int& Value);
		};

		struct ED_OUT Variant
		{
			friend Schema;
			friend Var;

		private:
			union Tag
			{
				char String[32];
				char* Pointer;
				int64_t Integer;
				double Number;
				bool Boolean;
			} Value;

		private:
			VarType Type;
			uint32_t Size;

		public:
			Variant() noexcept;
			Variant(const Variant& Other) noexcept;
			Variant(Variant&& Other) noexcept;
			~Variant() noexcept;
			bool Deserialize(const std::string& Value, bool Strict = false);
			std::string Serialize() const;
			std::string GetBlob() const;
			Decimal GetDecimal() const;
			void* GetPointer() const;
			const char* GetString() const;
			unsigned char* GetBinary() const;
			int64_t GetInteger() const;
			double GetNumber() const;
			bool GetBoolean() const;
			VarType GetType() const;
			size_t GetSize() const;
			Variant& operator= (const Variant& Other) noexcept;
			Variant& operator= (Variant&& Other) noexcept;
			bool operator== (const Variant& Other) const;
			bool operator!= (const Variant& Other) const;
			operator bool() const;
			bool IsString(const char* Value) const;
			bool IsObject() const;
			bool IsEmpty() const;
			bool Is(VarType Value) const;

		private:
			Variant(VarType NewType) noexcept;
			bool Same(const Variant& Value) const;
			void Copy(const Variant& Other);
			void Move(Variant&& Other);
			void Free();

		private:
			static size_t GetMaxSmallStringSize();
		};

		struct ED_OUT FileState
		{
			size_t Size = 0;
			size_t Links = 0;
			size_t Permissions = 0;
			size_t Document = 0;
			size_t Device = 0;
			size_t UserId = 0;
			size_t GroupId = 0;
			int64_t LastAccess = 0;
			int64_t LastPermissionChange = 0;
			int64_t LastModified = 0;
			bool Exists = false;
		};

		struct ED_OUT Timeout
		{
			std::chrono::microseconds Expires;
			SeqTaskCallback Callback;
			Difficulty Type;
			TaskId Id;
			size_t Invocations;
			bool Alive;

			Timeout(const SeqTaskCallback& NewCallback, const std::chrono::microseconds& NewTimeout, TaskId NewId, bool NewAlive, Difficulty NewType) noexcept;
			Timeout(SeqTaskCallback&& NewCallback, const std::chrono::microseconds& NewTimeout, TaskId NewId, bool NewAlive, Difficulty NewType) noexcept;
			Timeout(const Timeout& Other) noexcept;
			Timeout(Timeout&& Other) noexcept;
			Timeout& operator= (const Timeout& Other) noexcept;
			Timeout& operator= (Timeout&& Other) noexcept;
		};

		struct ED_OUT FileEntry
		{
			std::string Path;
			size_t Size = 0;
			int64_t LastModified = 0;
			int64_t CreationTime = 0;
			bool IsReferenced = false;
			bool IsDirectory = false;
			bool IsExists = false;
		};

		struct ED_OUT ChildProcess
		{
			friend class OS;

		private:
			bool Valid = false;
#ifdef ED_MICROSOFT
			void* Process = nullptr;
			void* Thread = nullptr;
			void* Job = nullptr;
#else
			pid_t Process;
#endif

		public:
			int64_t GetPid();
		};

		struct ED_OUT DateTime
		{
		private:
			std::chrono::system_clock::duration Time;
			struct tm DateValue;
			bool DateRebuild;

		public:
			DateTime() noexcept;
			DateTime(uint64_t Seconds) noexcept;
			DateTime(const DateTime& Value) noexcept;
			DateTime& operator= (const DateTime& Other) noexcept;
			void operator +=(const DateTime& Right);
			void operator -=(const DateTime& Right);
			bool operator >=(const DateTime& Right);
			bool operator <=(const DateTime& Right);
			bool operator >(const DateTime& Right);
			bool operator <(const DateTime& Right);
			bool operator ==(const DateTime& Right);
			std::string Format(const std::string& Value);
			std::string Date(const std::string& Value);
			std::string Iso8601();
			DateTime Now();
			DateTime FromNanoseconds(uint64_t Value);
			DateTime FromMicroseconds(uint64_t Value);
			DateTime FromMilliseconds(uint64_t Value);
			DateTime FromSeconds(uint64_t Value);
			DateTime FromMinutes(uint64_t Value);
			DateTime FromHours(uint64_t Value);
			DateTime FromDays(uint64_t Value);
			DateTime FromWeeks(uint64_t Value);
			DateTime FromMonths(uint64_t Value);
			DateTime FromYears(uint64_t Value);
			DateTime operator +(const DateTime& Right) const;
			DateTime operator -(const DateTime& Right) const;
			DateTime& SetDateSeconds(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateMinutes(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateHours(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateDay(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateWeek(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateMonth(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateYear(uint64_t Value, bool NoFlush = true);
			uint64_t DateSecond();
			uint64_t DateMinute();
			uint64_t DateHour();
			uint64_t DateDay();
			uint64_t DateWeek();
			uint64_t DateMonth();
			uint64_t DateYear();
			uint64_t Nanoseconds();
			uint64_t Microseconds();
			uint64_t Milliseconds();
			uint64_t Seconds();
			uint64_t Minutes();
			uint64_t Hours();
			uint64_t Days();
			uint64_t Weeks();
			uint64_t Months();
			uint64_t Years();

		private:
			void Rebuild();

		public:
			static std::string FetchWebDateGMT(int64_t TimeStamp);
			static std::string FetchWebDateTime(int64_t TimeStamp);
			static bool FetchWebDateGMT(char* Buffer, size_t Length, int64_t Time);
			static bool FetchWebDateTime(char* Buffer, size_t Length, int64_t Time);
			static int64_t ParseWebDate(const char* Date);
		};

		struct ED_OUT String
		{
		public:
			struct Settle
			{
				size_t Start = 0;
				size_t End = 0;
				bool Found = false;
			};

		private:
			std::string* Base;
			bool Deletable;

		public:
			String() noexcept;
			String(int Value) noexcept;
			String(unsigned int Value) noexcept;
			String(int64_t Value) noexcept;
			String(uint64_t Value) noexcept;
			String(float Value) noexcept;
			String(double Value) noexcept;
			String(long double Value) noexcept;
			String(const std::string& Buffer) noexcept;
			String(std::string* Buffer) noexcept;
			String(const std::string* Buffer) noexcept;
			String(const char* Buffer) noexcept;
			String(const char* Buffer, size_t Length) noexcept;
			String(String&& Value) noexcept;
			String(const String& Value) noexcept;
			~String() noexcept;
			String& EscapePrint();
			String& Escape();
			String& Unescape();
			String& Reserve(size_t Count = 1);
			String& Resize(size_t Count);
			String& Resize(size_t Count, char Char);
			String& Clear();
			String& ToUpper();
			String& ToLower();
			String& Clip(size_t Length);
			String& Compress(const char* SpaceIfNotFollowedOrPrecededByOf, const char* NotInBetweenOf, size_t Start = 0U);
			String& ReplaceOf(const char* Chars, const char* To, size_t Start = 0U);
			String& ReplaceNotOf(const char* Chars, const char* To, size_t Start = 0U);
			String& ReplaceGroups(const std::string& FromRegex, const std::string& To);
			String& Replace(const std::string& From, const std::string& To, size_t Start = 0U);
			String& Replace(const char* From, const char* To, size_t Start = 0U);
			String& Replace(const char& From, const char& To, size_t Position = 0U);
			String& Replace(const char& From, const char& To, size_t Position, size_t Count);
			String& ReplacePart(size_t Start, size_t End, const std::string& Value);
			String& ReplacePart(size_t Start, size_t End, const char* Value);
			String& ReplaceStartsWithEndsOf(const char* Begins, const char* EndsOf, const std::string& With, size_t Start = 0U);
			String& ReplaceInBetween(const char* Begins, const char* Ends, const std::string& With, bool Recursive, size_t Start = 0U);
			String& ReplaceNotInBetween(const char* Begins, const char* Ends, const std::string& With, bool Recursive, size_t Start = 0U);
			String& ReplaceParts(std::vector<std::pair<std::string, String::Settle>>& Inout, const std::string& With, const std::function<char(const std::string&, char, int)>& Surrounding = nullptr);
			String& ReplaceParts(std::vector<String::Settle>& Inout, const std::string& With, const std::function<char(char, int)>& Surrounding = nullptr);
			String& RemovePart(size_t Start, size_t End);
			String& Reverse();
			String& Reverse(size_t Start, size_t End);
			String& Substring(size_t Start);
			String& Substring(size_t Start, size_t Count);
			String& Substring(const String::Settle& Result);
			String& Splice(size_t Start, size_t End);
			String& Trim();
			String& Fill(const char& Char);
			String& Fill(const char& Char, size_t Count);
			String& Fill(const char& Char, size_t Start, size_t Count);
			String& Assign(const char* Raw);
			String& Assign(const char* Raw, size_t Length);
			String& Assign(const std::string& Raw);
			String& Assign(const std::string& Raw, size_t Start, size_t Count);
			String& Assign(const char* Raw, size_t Start, size_t Count);
			String& Append(const char* Raw);
			String& Append(const char& Char);
			String& Append(const char& Char, size_t Count);
			String& Append(const std::string& Raw);
			String& Append(const char* Raw, size_t Count);
			String& Append(const char* Raw, size_t Start, size_t Count);
			String& Append(const std::string& Raw, size_t Start, size_t Count);
			String& fAppend(const char* Format, ...);
			String& Insert(const std::string& Raw, size_t Position);
			String& Insert(const std::string& Raw, size_t Position, size_t Start, size_t Count);
			String& Insert(const std::string& Raw, size_t Position, size_t Count);
			String& Insert(const char& Char, size_t Position, size_t Count);
			String& Insert(const char& Char, size_t Position);
			String& Erase(size_t Position);
			String& Erase(size_t Position, size_t Count);
			String& EraseOffsets(size_t Start, size_t End);
			String& Eval(const std::string& Net, const std::string& Dir);
			std::vector<std::pair<std::string, String::Settle>> FindInBetween(const char* Begins, const char* Ends, const char* NotInSubBetweenOf, size_t Offset = 0U) const;
			std::vector<std::pair<std::string, String::Settle>> FindStartsWithEndsOf(const char* Begins, const char* EndsOf, const char* NotInSubBetweenOf, size_t Offset = 0U) const;
			String::Settle ReverseFind(const std::string& Needle, size_t Offset = 0U) const;
			String::Settle ReverseFind(const char* Needle, size_t Offset = 0U) const;
			String::Settle ReverseFind(const char& Needle, size_t Offset = 0U) const;
			String::Settle ReverseFindUnescaped(const char& Needle, size_t Offset = 0U) const;
			String::Settle ReverseFindOf(const std::string& Needle, size_t Offset = 0U) const;
			String::Settle ReverseFindOf(const char* Needle, size_t Offset = 0U) const;
			String::Settle Find(const std::string& Needle, size_t Offset = 0U) const;
			String::Settle Find(const char* Needle, size_t Offset = 0U) const;
			String::Settle Find(const char& Needle, size_t Offset = 0U) const;
			String::Settle FindUnescaped(const char& Needle, size_t Offset = 0U) const;
			String::Settle FindOf(const std::string& Needle, size_t Offset = 0U) const;
			String::Settle FindOf(const char* Needle, size_t Offset = 0U) const;
			String::Settle FindNotOf(const std::string& Needle, size_t Offset = 0U) const;
			String::Settle FindNotOf(const char* Needle, size_t Offset = 0U) const;
			bool IsPrecededBy(size_t At, const char* Of) const;
			bool IsFollowedBy(size_t At, const char* Of) const;
			bool StartsWith(const std::string& Value, size_t Offset = 0U) const;
			bool StartsWith(const char* Value, size_t Offset = 0U) const;
			bool StartsOf(const char* Value, size_t Offset = 0U) const;
			bool StartsNotOf(const char* Value, size_t Offset = 0U) const;
			bool EndsWith(const std::string& Value) const;
			bool EndsOf(const char* Value) const;
			bool EndsNotOf(const char* Value) const;
			bool EndsWith(const char* Value) const;
			bool EndsWith(const char& Value) const;
			bool Empty() const;
			bool HasInteger() const;
			bool HasNumber() const;
			bool HasDecimal() const;
			bool ToBoolean() const;
			int ToInt() const;
			long ToLong() const;
			float ToFloat() const;
			unsigned int ToUInt() const;
			unsigned long ToULong() const;
			int64_t ToInt64() const;
			double ToDouble() const;
			long double ToLDouble() const;
			uint64_t ToUInt64() const;
			size_t Size() const;
			size_t Capacity() const;
			char* Value() const;
			const char* Get() const;
			std::string& R();
			std::wstring ToWide() const;
			std::vector<std::string> Split(const std::string& With, size_t Start = 0U) const;
			std::vector<std::string> Split(char With, size_t Start = 0U) const;
			std::vector<std::string> SplitMax(char With, size_t MaxCount, size_t Start = 0U) const;
			std::vector<std::string> SplitOf(const char* With, size_t Start = 0U) const;
			std::vector<std::string> SplitNotOf(const char* With, size_t Start = 0U) const;
			String& operator = (String&& New) noexcept;
			String& operator = (const String& New) noexcept;

		public:
			static bool IsDigit(char Char);
			static bool IsAlphabetic(char Char);
			static int CaseCompare(const char* Value1, const char* Value2);
			static int Match(const char* Pattern, const char* Text);
			static int Match(const char* Pattern, size_t Length, const char* Text);
			static std::string ToString(float Number);
			static std::string ToString(double Number);
			static void ConvertToWide(const char* Input, size_t InputSize, wchar_t* Output, size_t OutputSize);
		};

		struct ED_OUT_TS Spin
		{
		private:
			std::atomic_flag Atom;

		public:
			Spin() noexcept;
			void Lock();
			void Unlock();
		};

		class ED_OUT_TS Guard
		{
		public:
			class ED_OUT Loaded
			{
			public:
				friend Guard;

			private:
				Guard* Base;

			public:
				Loaded(Loaded&& Other) noexcept;
				Loaded& operator =(Loaded&& Other) noexcept;
				~Loaded() noexcept;
				void Close();
				operator bool() const;

			private:
				Loaded(Guard* NewBase) noexcept;
			};

			class ED_OUT Stored
			{
			public:
				friend Guard;

			private:
				Guard* Base;

			public:
				Stored(Stored&& Other) noexcept;
				Stored& operator =(Stored&& Other) noexcept;
				~Stored() noexcept;
				void Close();
				operator bool() const;

			private:
				Stored(Guard* NewBase) noexcept;
			};

		public:
			friend Loaded;
			friend Stored;

		private:
			std::condition_variable Condition;
			std::mutex Mutex;
			uint32_t Readers;
			uint32_t Writers;

		public:
			Guard() noexcept;
			Guard(const Guard& Other) = delete;
			Guard(Guard&& Other) = delete;
			~Guard() noexcept = default;
			Guard& operator =(const Guard& Other) = delete;
			Guard& operator =(Guard&& Other) = delete;
			Loaded TryLoad();
			Loaded Load();
			Stored TryStore();
			Stored Store();
			bool TryLoadLock();
			void LoadLock();
			void LoadUnlock();
			bool TryStoreLock();
			void StoreLock();
			void StoreUnlock();
		};

		class ED_OUT_TS Var
		{
		public:
			class ED_OUT Set
			{
			public:
				static Unique<Schema> Auto(Variant&& Value);
				static Unique<Schema> Auto(const Variant& Value);
				static Unique<Schema> Auto(const std::string& Value, bool Strict = false);
				static Unique<Schema> Null();
				static Unique<Schema> Undefined();
				static Unique<Schema> Object();
				static Unique<Schema> Array();
				static Unique<Schema> Pointer(void* Value);
				static Unique<Schema> String(const std::string& Value);
				static Unique<Schema> String(const char* Value, size_t Size);
				static Unique<Schema> Binary(const std::string& Value);
				static Unique<Schema> Binary(const unsigned char* Value, size_t Size);
				static Unique<Schema> Binary(const char* Value, size_t Size);
				static Unique<Schema> Integer(int64_t Value);
				static Unique<Schema> Number(double Value);
				static Unique<Schema> Decimal(const BigNumber& Value);
				static Unique<Schema> Decimal(BigNumber&& Value);
				static Unique<Schema> DecimalString(const std::string& Value);
				static Unique<Schema> Boolean(bool Value);
			};

		public:
			static Variant Auto(const std::string& Value, bool Strict = false);
			static Variant Null();
			static Variant Undefined();
			static Variant Object();
			static Variant Array();
			static Variant Pointer(void* Value);
			static Variant String(const std::string& Value);
			static Variant String(const char* Value, size_t Size);
			static Variant Binary(const std::string& Value);
			static Variant Binary(const unsigned char* Value, size_t Size);
			static Variant Binary(const char* Value, size_t Size);
			static Variant Integer(int64_t Value);
			static Variant Number(double Value);
			static Variant Decimal(const BigNumber& Value);
			static Variant Decimal(BigNumber&& Value);
			static Variant DecimalString(const std::string& Value);
			static Variant Boolean(bool Value);
		};

		class ED_OUT_TS Allocator
		{
		public:
			struct ED_OUT Context
			{
				const char* Source;
				const char* Function;
				const char* TypeName;
				int Line;

				Context();
				Context(const char* NewSource, const char* NewFunction, const char* NewTypeName, int NewLine);
			};

		public:
			bool Tracing = true;

		public:
			virtual ~Allocator() = default;
			virtual Unique<void> Allocate(Context&& Origin, size_t Size) noexcept = 0;
			virtual void Free(Unique<void> Address) noexcept = 0;
			virtual void Watch(Context&& Origin, void* Address) noexcept = 0;
			virtual void Unwatch(void* Address) noexcept = 0;
			virtual bool IsValid(void* Address) noexcept = 0;
		};

		class ED_OUT_TS DebugAllocator final : public Allocator
		{
		public:
			struct Block
			{
				std::string TypeName;
				Context Origin;
				time_t Time;
				size_t Size;
				bool Active;
			};

		private:
			std::unordered_map<void*, Block> Blocks;
			std::recursive_mutex Mutex;

		public:
			~DebugAllocator() noexcept override;
			Unique<void> Allocate(Context&& Origin, size_t Size) noexcept override;
			void Free(Unique<void> Address) noexcept override;
			void Watch(Context&& Origin, void* Address) noexcept override;
			void Unwatch(void* Address) noexcept override;
			bool IsValid(void* Address) noexcept override;
			bool Dump(void* Address);
			bool FindBlock(void* Address, Block* Output);
			const std::unordered_map<void*, Block>& GetBlocks() const;
		};

		class ED_OUT_TS PoolAllocator final : public Allocator
		{
		public:
			struct PageAddress
			{
				void* BaseAddress;
				size_t Size;
			};

			struct PageCache
			{
				 std::queue<void*> Pool;
				 int64_t AliveTime;
				 void* BaseAddress;
				 size_t Capacity;
			};

			struct PageGroup
			{
				std::unordered_map<void*, PageCache> Cache;
			};

		private:
			std::unordered_map<size_t, PageGroup> Pages;
			std::unordered_map<void*, PageAddress> Blocks;
			std::recursive_mutex Mutex;
			uint64_t MinimalLifeTime;
			double ElementsReducingFactor;
			size_t ElementsReducingBase;
			size_t ElementsPerAllocation;

		public:
			PoolAllocator(uint64_t MinimalLifeTimeMs = 2000, size_t MaxElementsPerAllocation = 1024, size_t ElementsReducingBaseBytes = 300, double ElementsReducingFactorRate = 5.0);
			~PoolAllocator() noexcept override;
			Unique<void> Allocate(Context && Origin, size_t Size) noexcept override;
			void Free(Unique<void> Address) noexcept override;
			void Watch(Context && Origin, void* Address) noexcept override;
			void Unwatch(void* Address) noexcept override;
			bool IsValid(void* Address) noexcept override;

		private:
			PageCache* AllocatePageCache(Context&& Origin, PageGroup& Page, size_t Size);
			PageCache* GetPageCache(PageGroup& Page);
			size_t GetElementsCount(PageGroup& Page, size_t Size);
			double GetFrequency(PageGroup& Page);
		};

		class ED_OUT_TS DefaultAllocator final : public Allocator
		{
		public:
			~DefaultAllocator() override = default;
			Unique<void> Allocate(Context&& Origin, size_t Size) noexcept override;
			void Free(Unique<void> Address) noexcept override;
			void Watch(Context&& Origin, void* Address) noexcept override;
			void Unwatch(void* Address) noexcept override;
			bool IsValid(void* Address) noexcept override;
		};

		class ED_OUT_TS OS
		{
		public:
			class ED_OUT CPU
			{
			public:
				enum class Arch
				{
					X64,
					ARM,
					Itanium,
					X86,
					Unknown,
				};

				enum class Endian
				{
					Little,
					Big,
				};

				enum class Cache
				{
					Unified,
					Instruction,
					Data,
					Trace,
				};

				struct QuantityInfo
				{
					uint32_t Logical;
					uint32_t Physical;
					uint32_t Packages;
				};

				struct CacheInfo
				{
					size_t Size;
					size_t LineSize;
					uint8_t Associativity;
					Cache Type;
				};

			public:
				static QuantityInfo GetQuantityInfo();
				static CacheInfo GetCacheInfo(unsigned int level);
				static Arch GetArch() noexcept;
				static Endian GetEndianness() noexcept;
				static size_t GetFrequency() noexcept;

			public:
				template <typename T>
				static typename std::enable_if<std::is_arithmetic<T>::value, T>::type SwapEndianness(T Value)
				{
					union U
					{
						T Value;
						std::array<std::uint8_t, sizeof(T)> Data;
					} From, To;

					From.Value = Value;
					std::reverse_copy(From.Data.begin(), From.Data.end(), To.Data.begin());
					return To.Value;
				}
				template <typename T>
				static typename std::enable_if<std::is_arithmetic<T>::value, T>::type ToEndianness(Endian Type, T Value)
				{
					return GetEndianness() == Type ? Value : SwapEndianness(Value);
				}
			};

			class ED_OUT Directory
			{
			public:
				static void Set(const char* Path);
				static void Patch(const std::string& Path);
				static bool Scan(const std::string& Path, std::vector<FileEntry>* Entries);
				static bool Each(const char* Path, const std::function<bool(FileEntry*)>& Callback);
				static bool Create(const char* Path);
				static bool Remove(const char* Path);
				static bool IsExists(const char* Path);
				static std::string Get();
				static std::vector<std::string> GetMounts();
			};

			class ED_OUT File
			{
			public:
				static bool Write(const std::string& Path, const char* Data, size_t Length);
				static bool Write(const std::string& Path, const std::string& Data);
				static bool State(const std::string& Path, FileEntry* Resource);
				static bool Move(const char* From, const char* To);
				static bool Remove(const char* Path);
				static bool IsExists(const char* Path);
				static void Close(Unique<void> Stream);
				static int Compare(const std::string& FirstPath, const std::string& SecondPath);
				static size_t Join(const std::string& To, const std::vector<std::string>& Paths);
				static uint64_t GetCheckSum(const std::string& Data);
				static FileState GetProperties(const char* Path);
				static Unique<Stream> OpenJoin(const std::string& Path, const std::vector<std::string>& Paths);
				static Unique<Stream> OpenArchive(const std::string& Path, size_t UnarchivedMaxSize = 128 * 1024 * 1024);
				static Unique<Stream> Open(const std::string& Path, FileMode Mode, bool Async = false);
				static Unique<void> Open(const char* Path, const char* Mode);
				static Unique<unsigned char> ReadChunk(Stream* Stream, size_t Length);
				static Unique<unsigned char> ReadAll(const std::string& Path, size_t* ByteLength);
				static Unique<unsigned char> ReadAll(Stream* Stream, size_t* ByteLength);
				static std::string ReadAsString(const std::string& Path);
				static std::vector<std::string> ReadAsArray(const std::string& Path);
			};

			class ED_OUT Path
			{
			public:
				static bool IsRemote(const char* Path);
				static std::string Resolve(const char* Path);
				static std::string Resolve(const std::string& Path, const std::string& Directory);
				static std::string ResolveDirectory(const char* Path);
				static std::string ResolveDirectory(const std::string& Path, const std::string& Directory);
				static std::string GetDirectory(const char* Path, size_t Level = 0);
				static std::string GetNonExistant(const std::string& Path);
				static const char* GetFilename(const char* Path);
				static const char* GetExtension(const char* Path);
			};

			class ED_OUT Net
			{
			public:
				static bool GetETag(char* Buffer, size_t Length, FileEntry* Resource);
				static bool GetETag(char* Buffer, size_t Length, int64_t LastModified, size_t ContentLength);
				static socket_t GetFd(FILE* Stream);
			};

			class ED_OUT Process
			{
            public:
                struct ArgsContext
                {
                    std::unordered_map<std::string, std::string> Base;
                    
                    ArgsContext(int Argc, char** Argv, const std::string& WhenNoValue = "1") noexcept
                    {
                        Base = OS::Process::GetArgs(Argc, Argv, WhenNoValue);
                    }
                    void ForEach(const std::function<void(const std::string&, const std::string&)>& Callback) const
                    {
                        ED_ASSERT_V(Callback != nullptr, "callback should not be empty");
                        for (auto& Item : Base)
                            Callback(Item.first, Item.second);
                    }
                    bool IsEnabled(const std::string& Option, const std::string& Shortcut = "") const
                    {
                        auto It = Base.find(Option);
                        if (It == Base.end() || !IsTrue(It->second))
                            return Shortcut.empty() ? false : IsEnabled(Shortcut);
                            
                        return true;
                    }
                    bool IsDisabled(const std::string& Option, const std::string& Shortcut = "") const
                    {
                        auto It = Base.find(Option);
                        if (It == Base.end())
                            return Shortcut.empty() ? true : IsDisabled(Shortcut);
                            
                        return IsFalse(It->second);
                    }
                    bool Has(const std::string& Option, const std::string& Shortcut = "") const
                    {
                        if (Base.find(Option) != Base.end())
                            return true;
                        
                        return Shortcut.empty() ? false : Base.find(Shortcut) != Base.end();
                    }
                    std::string& Get(const std::string& Option, const std::string& Shortcut = "")
                    {
                        if (Base.find(Option) != Base.end())
                            return Base[Option];
                        
                        return Shortcut.empty() ? Base[Option] : Base[Shortcut];
                    }
                    std::string& GetIf(const std::string& Option, const std::string& Shortcut, const std::string& WhenEmpty)
                    {
                        if (Base.find(Option) != Base.end())
                            return Base[Option];
                        
                        if (!Shortcut.empty() && Base.find(Shortcut) != Base.end())
                            return Base[Shortcut];
                        
                        std::string& Value = Base[Option];
                        Value = WhenEmpty;
                        return Value;
                    }
                    std::string& GetAppPath()
                    {
                        return Get("__path__");
                    }
                    
                private:
                    bool IsTrue(const std::string& Value) const
                    {
                        if (Value.empty())
                            return false;
                        
                        if (String((std::string*)&Value).ToUInt64() > 0)
                            return true;
                        
                        String Data(Value);
                        Data.ToLower();
                        return Data.R() == "on" || Data.R() == "true" || Data.R() == "yes" || Data.R() == "y";
                    }
                    bool IsFalse(const std::string& Value) const
                    {
                        if (Value.empty())
                            return true;
                        
                        if (String((std::string*)&Value).ToUInt64() > 0)
                            return false;
                        
                        String Data(Value);
                        Data.ToLower();
                        return Data.R() == "off" || Data.R() == "false" || Data.R() == "no" || Data.R() == "n";
                    }
                };
                
			public:
				static void Interrupt();
				static int Execute(const char* Format, ...);
				static bool Spawn(const std::string& Path, const std::vector<std::string>& Params, ChildProcess* Result);
				static bool Await(ChildProcess* Process, int* ExitCode);
				static bool Free(ChildProcess* Process);
                static std::string GetThreadId(const std::thread::id& Id);
                static std::unordered_map<std::string, std::string> GetArgs(int Argc, char** Argv, const std::string& WhenNoValue = "1");
			};

			class ED_OUT Symbol
			{
			public:
				static Unique<void> Load(const std::string& Path = "");
				static Unique<void> LoadFunction(void* Handle, const std::string& Name);
				static bool Unload(void* Handle);
			};

			class ED_OUT Input
			{
			public:
				static bool Text(const std::string& Title, const std::string& Message, const std::string& DefaultInput, std::string* Result);
				static bool Password(const std::string& Title, const std::string& Message, std::string* Result);
				static bool Save(const std::string& Title, const std::string& DefaultPath, const std::string& Filter, const std::string& FilterDescription, std::string* Result);
				static bool Open(const std::string& Title, const std::string& DefaultPath, const std::string& Filter, const std::string& FilterDescription, bool Multiple, std::string* Result);
				static bool Folder(const std::string& Title, const std::string& DefaultPath, std::string* Result);
				static bool Color(const std::string& Title, const std::string& DefaultHexRGB, std::string* Result);
			};

			class ED_OUT Error
			{
			public:
				static int Get();
				static std::string GetName(int Code);
				static bool IsError(int Code);
			};

			class ED_OUT Timing
			{
			public:
				struct ED_OUT Tick
				{
					bool IsCounting;

					Tick(bool Active) noexcept;
					Tick(const Tick& Other) = delete;
					Tick(Tick&& Other) noexcept;
					~Tick() noexcept;
					Tick& operator =(const Tick& Other) = delete;
					Tick& operator =(Tick&& Other) noexcept;
				};

			public:
				static Tick Measure(const char* File, const char* Function, int Line, uint64_t ThresholdMS);
				static void MeasureLoop();
				static std::string GetMeasureTrace();
			};

		public:
			struct ED_OUT Message
			{
				char Buffer[ED_BIG_CHUNK_SIZE] = { '\0' };
				char Date[64] = { '\0' };
				std::string Temp;
				const char* Source;
				int Level;
				int Line;
				int Size;
				bool Pretty;
				bool Fatal;

				const char* GetLevelName() const;
				StdColor GetLevelColor() const;
				std::string& GetText();
			};

		private:
			static std::function<void(Message&)> Callback;
			static std::mutex Buffer;
			static Allocator* Memory;
            static bool Pretty;
			static bool Deferred;
			static bool Active;

		public:
			static Unique<void> Malloc(size_t Size) noexcept;
			static Unique<void> MallocDebug(size_t Size, Allocator::Context&& Origin) noexcept;
			static void Free(Unique<void> Address) noexcept;
			static void Watch(void* Address, Allocator::Context&& Origin) noexcept;
			static void Unwatch(void* Address) noexcept;
			static void Assert(bool Fatal, int Line, const char* Source, const char* Function, const char* Condition, const char* Format, ...);
			static void Log(int Level, int Line, const char* Source, const char* Format, ...);
			static void Pause();
			static void SetAllocator(Allocator* NewAllocator);
			static void SetLogCallback(const std::function<void(Message&)>& Callback);
			static void SetLogActive(bool Enabled);
			static void SetLogDeferred(bool Enabled);
            static void SetLogPretty(bool Enabled);
			static void SetMemoryTracing(bool Enabled);
			static bool IsLogActive();
			static bool IsLogDeferred();
			static bool IsLogPretty();
			static bool IsValidAddress(void* Address);
			static Allocator* GetAllocator();
			static std::string GetStackTrace(size_t Skips, size_t MaxFrames = 16);

		private:
			static void EnqueueLog(Message&& Data);
			static void DispatchLog(Message& Data);
			static void PrettyPrintLog(Console* Log, const char* Buffer, StdColor BaseColor);
		};

		class ED_OUT_TS Composer
		{
		private:
			static Mapping<std::unordered_map<uint64_t, std::pair<uint64_t, void*>>>* Factory;

		public:
			static bool Clear();
			static bool Pop(const std::string& Hash);
			static std::unordered_set<uint64_t> Fetch(uint64_t Id);

		private:
			static void Push(uint64_t TypeId, uint64_t Tag, void* Callback);
			static void* Find(uint64_t TypeId);

		public:
			template <typename T, typename... Args>
			static Unique<T> Create(const std::string& Hash, Args... Data)
			{
				return Create<T, Args...>(ED_HASH(Hash), Data...);
			}
			template <typename T, typename... Args>
			static Unique<T> Create(uint64_t Id, Args... Data)
			{
				void* (*Callable)(Args...) = nullptr;
				reinterpret_cast<void*&>(Callable) = Find(Id);

				if (!Callable)
					return nullptr;

				return (T*)Callable(Data...);
			}
			template <typename T, typename... Args>
			static void Push(uint64_t Tag)
			{
				auto Callable = &Composer::Callee<T, Args...>;
				void* Result = reinterpret_cast<void*&>(Callable);
				Push(T::GetTypeId(), Tag, Result);
			}

		private:
			template <typename T, typename... Args>
			static Unique<void> Callee(Args... Data)
			{
				return (void*)new T(Data...);
			}
		};

		template <typename T>
		class ED_OUT_TS Reference
		{
		private:
			std::atomic<uint32_t> __vcnt = 1;

		public:
			void operator delete(void* Ptr) noexcept
			{
				if (Ptr != nullptr)
				{
					auto Handle = (T*)Ptr;
					ED_ASSERT_V(Handle->__vcnt <= 1, "address at 0x%" PRIXPTR " is still in use but destructor has been called by delete as %s at %s()", Ptr, typeid(T).name(), __func__);
					ED_FREE(Ptr);
				}
			}
			void* operator new(size_t Size) noexcept
			{
				return (void*)ED_MALLOC(T, Size);
			}
			bool IsMarkedRef() const noexcept
			{
				return (bool)(__vcnt.load() & 0x80000000);
			}
			uint32_t GetRefCount() const noexcept
			{
				return __vcnt.load() & 0x7FFFFFFF;
			}
			void MarkRef() noexcept
			{
				__vcnt |= 0x80000000;
			}
			void AddRef() noexcept
			{
				__vcnt = (__vcnt.load() & 0x7FFFFFFF) + 1;
			}
			void Release() noexcept
			{
				__vcnt &= 0x7FFFFFFF;
				ED_ASSERT_V(__vcnt > 0 && OS::IsValidAddress((void*)(T*)this), "address at 0x%" PRIXPTR " has already been released as %s at %s()", (void*)this, typeid(T).name(), __func__);
				if (!--__vcnt)
					delete (T*)this;
			}
		};

		template <typename T>
		class ED_OUT_TS UPtr
		{
		private:
			T* Pointer;

		public:
			UPtr(T* NewPointer) noexcept : Pointer(NewPointer)
			{
			}
			UPtr(const UPtr& Other) noexcept = delete;
			UPtr(UPtr&& Other) noexcept : Pointer(Other.Pointer)
			{
				Other.Pointer = nullptr;
			}
			~UPtr()
			{
				Cleanup<T>();
			}
			UPtr& operator= (const UPtr& Other) noexcept = delete;
			UPtr& operator= (UPtr&& Other) noexcept
			{
				if (this == &Other)
					return *this;

				Cleanup<T>();
				Pointer = Other.Pointer;
				Other.Pointer = nullptr;
				return *this;
			}
			T* operator-> ()
			{
				ED_ASSERT(Pointer != nullptr, nullptr, "null pointer access");
				return Pointer;
			}
			operator T* ()
			{
				return Pointer;
			}
			Unique<T> Reset()
			{
				T* Result = Pointer;
				Pointer = nullptr;
				return Result;
			}

		private:
			template <typename Q>
			inline typename std::enable_if<std::is_trivially_default_constructible<Q>::value && !std::is_base_of<Reference<Q>, Q>::value, void>::type Cleanup()
			{
				ED_FREE(Pointer);
				Pointer = nullptr;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_trivially_default_constructible<Q>::value && !std::is_base_of<Reference<Q>, Q>::value, void>::type Cleanup()
			{
				ED_DELETE(T, Pointer);
				Pointer = nullptr;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_trivially_default_constructible<Q>::value && std::is_base_of<Reference<Q>, Q>::value, void>::type Cleanup()
			{
				ED_CLEAR(Pointer);
			}
		};

		template <typename T>
		class Reactive
		{
		public:
			typedef std::function<void(const T&)> SimpleCallback;
			typedef std::function<void(const T&, const T&)> ComplexCallback;

		private:
			std::unordered_map<size_t, ComplexCallback> Listeners;
			size_t Index;

		private:
			T Value;

		public:
			Reactive() : Index(0)
			{
			}
			Reactive(const T& NewValue) noexcept : Index(0), Value(NewValue)
			{
			}
			Reactive(T&& NewValue) noexcept : Index(0), Value(std::move(NewValue))
			{
			}
			Reactive(const Reactive& Other) = delete;
			Reactive(Reactive&& Other) = default;
			~Reactive() = default;
			Reactive& operator= (const T& Other)
			{
				Notify(Other);
				Value = Other;
				return *this;
			}
			Reactive& operator= (T&& Other)
			{
				Notify(Other);
				Value = std::move(Other);
				return *this;
			}
			Reactive& operator= (const Reactive& Other) = delete;
			Reactive& operator= (Reactive&& Other) = default;
			operator const T& () const
			{
				return Value;
			}
			size_t Listen(ComplexCallback&& NewCallback)
			{
				size_t Id = Index++;
				Listeners[Id] = std::move(NewCallback);
				return Id;
			}
			size_t Listen(const ComplexCallback& NewCallback)
			{
				size_t Id = Index++;
				Listeners[Id] = NewCallback;
				return Id;
			}
			size_t Listen(SimpleCallback&& NewCallback)
			{
				return Listen([NewCallback = std::move(NewCallback)](const T& New, const T&) { NewCallback(New); });
			}
			size_t Listen(const SimpleCallback& NewCallback)
			{
				return Listen([NewCallback](const T& New, const T&) { NewCallback(New); });
			}
			void Unlisten(size_t Id)
			{
				Listeners.erase(Id);
			}
			void Notify(const T& Other)
			{
				for (auto& Item : Listeners)
					Item.second(Other, Value);
			}
			void Notify()
			{
				Notify(Value);
			}

		public:
			inline typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<T, std::string>::value || std::is_same<T, std::string>::value, Reactive&>::type operator+= (const T& Other)
			{
				T Temp = Value + Other;
				Notify(Temp);
				Value = Temp;
				return *this;
			}
		};

		template <typename T>
		class Reactive<T*>
		{
		public:
			typedef std::function<void(T*)> SimpleCallback;
			typedef std::function<void(T*, T*)> ComplexCallback;

		private:
			std::unordered_map<size_t, ComplexCallback> Listeners;
			size_t Index;

		private:
			UPtr<T> Value;

		public:
			Reactive() : Index(0), Value(nullptr)
			{
			}
			Reactive(T* NewValue) noexcept : Index(0), Value(NewValue)
			{
			}
			Reactive(const Reactive& Other) = delete;
			Reactive(Reactive&& Other) = default;
			~Reactive() = default;
			Reactive& operator= (T* Other)
			{
				Notify(Other);
				Value = Other;
				return *this;
			}
			Reactive& operator= (UPtr<T>&& Other)
			{
				Notify(Other);
				Value = std::move(Other);
				return *this;
			}
			Reactive& operator= (const Reactive& Other) = delete;
			Reactive& operator= (Reactive&& Other) = default;
			T* operator-> ()
			{
				ED_ASSERT(Value != nullptr, nullptr, "null pointer access");
				return Value;
			}
			operator T* ()
			{
				return Value;
			}
			Unique<T> Reset()
			{
				return Value.Reset();
			}
			size_t Listen(ComplexCallback&& NewCallback)
			{
				size_t Id = Index++;
				Listeners[Id] = std::move(NewCallback);
				return Id;
			}
			size_t Listen(const ComplexCallback& NewCallback)
			{
				size_t Id = Index++;
				Listeners[Id] = NewCallback;
				return Id;
			}
			size_t Listen(SimpleCallback&& NewCallback)
			{
				return Listen([NewCallback = std::move(NewCallback)](T* New, T*) { NewCallback(New); });
			}
			size_t Listen(const SimpleCallback& NewCallback)
			{
				return Listen([NewCallback](T* New, T*) { NewCallback(New); });
			}
			void Unlisten(size_t Id)
			{
				Listeners.erase(Id);
			}
			void Notify(T* Other)
			{
				for (auto& Item : Listeners)
					Item.second(Other, Value);
			}
			void Notify()
			{
				Notify(Value);
			}
		};

		class ED_OUT_TS Console final : public Reference<Console>
		{
		protected:
			std::mutex Session;
			std::mutex Lock;
			bool Coloring;
			bool Allocated;
			bool Present;
			double Time;
#ifdef ED_MICROSOFT
			FILE* Conin;
			FILE* Conout;
			FILE* Conerr;
			unsigned short Attributes;
#endif
		private:
			Console() noexcept;

		public:
			~Console() noexcept;
			void Begin();
			void End();
			void Hide();
			void Show();
			void Clear();
			void Flush();
			void FlushWrite();
			void Trace(uint32_t MaxFrames = 32);
			void CaptureTime();
			void SetColoring(bool Enabled);
			void SetCursor(uint32_t X, uint32_t Y);
			void ColorBegin(StdColor Text, StdColor Background = StdColor::Black);
			void ColorEnd();
			void WriteBuffer(const char* Buffer);
			void WriteLine(const std::string& Line);
			void WriteChar(char Value);
			void Write(const std::string& Line);
			void fWriteLine(const char* Format, ...);
			void fWrite(const char* Format, ...);
			void sWriteLine(const std::string& Line);
			void sWrite(const std::string& Line);
			void sfWriteLine(const char* Format, ...);
			void sfWrite(const char* Format, ...);
			void GetSize(uint32_t* Width, uint32_t* Height);
			double GetCapturedTime() const;
			std::string Read(size_t Size);

		public:
			static Console* Get();
			static bool Reset();
			static bool IsPresent();

		private:
			static Console* Singleton;
		};

		class ED_OUT Timer final : public Reference<Timer>
		{
		public:
			typedef std::chrono::microseconds Units;

		public:
			struct Capture
			{
				const char* Name;
				Units Begin = Units(0);
				Units End = Units(0);
				Units Delta = Units(0);
				float Step = 0.0;
			};

		private:
			struct
			{
				Units Begin = Units(0);
				Units When = Units(0);
				Units Delta = Units(0);
			} Timing;

			struct
			{
				Units When = Units(0);
				Units Delta = Units(0);
				Units Sum = Units(0);
				bool InFrame = false;
			} Fixed;

		private:
			std::queue<Capture> Captures;
			Units MinDelta = Units(0);
			Units MaxDelta = Units(0);
			float FixedFrames = 0.0f;
			float MaxFrames = 0.0f;

		public:
			Timer() noexcept;
			~Timer() noexcept = default;
			void SetFixedFrames(float Value);
			void SetMaxFrames(float Value);
			void Begin();
			void Finish();
			void Push(const char* Name = nullptr);
			bool PopIf(float GreaterThan, Capture* Out = nullptr);
			Capture Pop();
			float GetMaxFrames() const;
			float GetMinStep() const;
			float GetFrames() const;
			float GetElapsed() const;
			float GetElapsedMills() const;
			float GetStep() const;
			float GetFixedStep() const;
			bool IsFixed() const;

		public:
			static float ToSeconds(const Units& Value);
			static float ToMills(const Units& Value);
			static Units ToUnits(float Value);
			static Units Clock();
		};

		class ED_OUT Stream : public Reference<Stream>
		{
		protected:
			std::string Path;
			size_t VirtualSize;

		public:
			Stream() noexcept;
			virtual ~Stream() noexcept = default;
			virtual void Clear() = 0;
			virtual bool Open(const char* File, FileMode Mode) = 0;
			virtual bool Close() = 0;
			virtual bool Seek(FileSeek Mode, int64_t Offset) = 0;
			virtual bool Move(int64_t Offset) = 0;
			virtual int Flush() = 0;
			virtual size_t ReadAny(const char* Format, ...) = 0;
			virtual size_t Read(char* Buffer, size_t Length) = 0;
			virtual size_t WriteAny(const char* Format, ...) = 0;
			virtual size_t Write(const char* Buffer, size_t Length) = 0;
			virtual size_t Tell() = 0;
			virtual int GetFd() const = 0;
			virtual void* GetBuffer() const = 0;
			void SetVirtualSize(size_t Size);
			size_t ReadAll(const std::function<void(char*, size_t)>& Callback);
			size_t GetVirtualSize() const;
			size_t GetSize();
			std::string& GetSource();
		};

		class ED_OUT FileStream : public Stream
		{
		protected:
			FILE* Resource;

		public:
			FileStream() noexcept;
			~FileStream() noexcept override;
			void Clear() override;
			bool Open(const char* File, FileMode Mode) override;
			bool Close() override;
			bool Seek(FileSeek Mode, int64_t Offset) override;
			bool Move(int64_t Offset) override;
			int Flush() override;
			size_t ReadAny(const char* Format, ...) override;
			size_t Read(char* Buffer, size_t Length) override;
			size_t WriteAny(const char* Format, ...) override;
			size_t Write(const char* Buffer, size_t Length) override;
			size_t Tell() override;
			int GetFd() const override;
			void* GetBuffer() const override;
		};

		class ED_OUT GzStream : public Stream
		{
		protected:
			void* Resource;

		public:
			GzStream() noexcept;
			~GzStream() noexcept override;
			void Clear() override;
			bool Open(const char* File, FileMode Mode) override;
			bool Close() override;
			bool Seek(FileSeek Mode, int64_t Offset) override;
			bool Move(int64_t Offset) override;
			int Flush() override;
			size_t ReadAny(const char* Format, ...) override;
			size_t Read(char* Buffer, size_t Length) override;
			size_t WriteAny(const char* Format, ...) override;
			size_t Write(const char* Buffer, size_t Length) override;
			size_t Tell() override;
			int GetFd() const override;
			void* GetBuffer() const override;
		};

		class ED_OUT WebStream : public Stream
		{
		protected:
			void* Resource;
			std::unordered_map<std::string, std::string> Headers;
			std::vector<char> Chunk;
			size_t Offset;
			size_t Size;
			bool Async;

		public:
			WebStream(bool IsAsync) noexcept;
			WebStream(bool IsAsync, std::unordered_map<std::string, std::string>&& NewHeaders) noexcept;
			~WebStream() noexcept override;
			void Clear() override;
			bool Open(const char* File, FileMode Mode) override;
			bool Close() override;
			bool Seek(FileSeek Mode, int64_t Offset) override;
			bool Move(int64_t Offset) override;
			int Flush() override;
			size_t ReadAny(const char* Format, ...) override;
			size_t Read(char* Buffer, size_t Length) override;
			size_t WriteAny(const char* Format, ...) override;
			size_t Write(const char* Buffer, size_t Length) override;
			size_t Tell() override;
			int GetFd() const override;
			void* GetBuffer() const override;
		};

		class ED_OUT FileLog final : public Reference<FileLog>
		{
		private:
			std::string LastValue;
			size_t Offset;
			int64_t Time;

		public:
			Stream* Source = nullptr;
			std::string Path, Name;

		public:
			FileLog(const std::string& Root) noexcept;
			~FileLog() noexcept;
			void Process(const std::function<bool(FileLog*, const char*, int64_t)>& Callback);
		};

		class ED_OUT FileTree final : public Reference<FileTree>
		{
		public:
			std::vector<FileTree*> Directories;
			std::vector<std::string> Files;
			std::string Path;

		public:
			FileTree(const std::string& Path) noexcept;
			~FileTree() noexcept;
			void Loop(const std::function<bool(const FileTree*)>& Callback) const;
			const FileTree* Find(const std::string& Path) const;
			size_t GetFiles() const;
		};

		class ED_OUT Costate final : public Reference<Costate>
		{
			friend Cocontext;

		private:
			std::unordered_set<Coroutine*> Cached;
			std::unordered_set<Coroutine*> Used;
			std::thread::id Thread;
			Coroutine* Current;
			Cocontext* Master;
			size_t Size;

		public:
			std::function<void()> NotifyLock;
			std::function<void()> NotifyUnlock;

		public:
			Costate(size_t StackSize = ED_STACK_SIZE) noexcept;
			~Costate() noexcept;
			Costate(const Costate&) = delete;
			Costate(Costate&&) = delete;
			Costate& operator= (const Costate&) = delete;
			Costate& operator= (Costate&&) = delete;
			Coroutine* Pop(const TaskCallback& Procedure);
			Coroutine* Pop(TaskCallback&& Procedure);
			int Reuse(Coroutine* Routine, const TaskCallback& Procedure);
			int Reuse(Coroutine* Routine, TaskCallback&& Procedure);
			int Reuse(Coroutine* Routine);
			int Push(Coroutine* Routine);
			int Activate(Coroutine* Routine);
			int Deactivate(Coroutine* Routine);
			int Deactivate(Coroutine* Routine, const TaskCallback& AfterSuspend);
			int Deactivate(Coroutine* Routine, TaskCallback&& AfterSuspend);
			int Resume(Coroutine* Routine);
			int Dispatch();
			int Suspend();
			void Clear();
			bool HasActive() const;
			Coroutine* GetCurrent() const;
			size_t GetCount() const;

		private:
			int Swap(Coroutine* Routine);

		public:
			static Costate* Get();
			static Coroutine* GetCoroutine();
			static bool GetState(Costate** State, Coroutine** Routine);
			static bool IsCoroutine();

		public:
			static void ED_COCALL ExecutionEntry(ED_CODATA);
		};

		class ED_OUT Schema final : public Reference<Schema>
		{
		protected:
			std::vector<Schema*>* Nodes;
			Schema* Parent;
			bool Saved;

		public:
			std::string Key;
			Variant Value;

		public:
			Schema(const Variant& Base) noexcept;
			Schema(Variant&& Base) noexcept;
			~Schema() noexcept;
			std::unordered_map<std::string, size_t> GetNames() const;
			std::vector<Schema*> FindCollection(const std::string& Name, bool Deep = false) const;
			std::vector<Schema*> FetchCollection(const std::string& Notation, bool Deep = false) const;
			std::vector<Schema*> GetAttributes() const;
			std::vector<Schema*>& GetChilds();
			Schema* Find(const std::string& Name, bool Deep = false) const;
			Schema* Fetch(const std::string& Notation, bool Deep = false) const;
			Variant FetchVar(const std::string& Key, bool Deep = false) const;
			Variant GetVar(size_t Index) const;
			Variant GetVar(const std::string& Key) const;
			Variant GetAttributeVar(const std::string& Key) const;
			Schema* GetParent() const;
			Schema* GetAttribute(const std::string& Key) const;
			Schema* Get(size_t Index) const;
			Schema* Get(const std::string& Key) const;
			Schema* Set(const std::string& Key);
			Schema* Set(const std::string& Key, const Variant& Value);
			Schema* Set(const std::string& Key, Variant&& Value);
			Schema* Set(const std::string& Key, Unique<Schema> Value);
			Schema* SetAttribute(const std::string& Key, const Variant& Value);
			Schema* SetAttribute(const std::string& Key, Variant&& Value);
			Schema* Push(const Variant& Value);
			Schema* Push(Variant&& Value);
			Schema* Push(Unique<Schema> Value);
			Schema* Pop(size_t Index);
			Schema* Pop(const std::string& Name);
			Unique<Schema> Copy() const;
			bool Rename(const std::string& Name, const std::string& NewName);
			bool Has(const std::string& Name) const;
			bool HasAttribute(const std::string& Name) const;
			bool IsEmpty() const;
			bool IsAttribute() const;
			bool IsSaved() const;
			size_t Size() const;
			std::string GetName() const;
			void Join(Schema* Other, bool Copy = true, bool Fast = true);
			void Reserve(size_t Size);
			void Unlink();
			void Clear();
			void Save();

		protected:
			void Allocate();
			void Allocate(const std::vector<Schema*>& Other);

		private:
			void Attach(Schema* Root);

		public:
			static bool Transform(Schema* Value, const SchemaNameCallback& Callback);
			static bool ConvertToXML(Schema* Value, const SchemaWriteCallback& Callback);
			static bool ConvertToJSON(Schema* Value, const SchemaWriteCallback& Callback);
			static bool ConvertToJSONB(Schema* Value, const SchemaWriteCallback& Callback);
			static std::string ToXML(Schema* Value);
			static std::string ToJSON(Schema* Value);
			static std::vector<char> ToJSONB(Schema* Value);
			static Unique<Schema> ConvertFromXML(const char* Buffer, bool Assert = true);
			static Unique<Schema> ConvertFromJSON(const char* Buffer, size_t Size, bool Assert = true);
			static Unique<Schema> ConvertFromJSONB(const SchemaReadCallback& Callback, bool Assert = true);
			static Unique<Schema> FromXML(const std::string& Text, bool Assert = true);
			static Unique<Schema> FromJSON(const std::string& Text, bool Assert = true);
			static Unique<Schema> FromJSONB(const std::vector<char>& Binary, bool Assert = true);

		private:
			static bool ProcessConvertionFromXML(void* Base, Schema* Current);
			static bool ProcessConvertionFromJSON(void* Base, Schema* Current);
			static bool ProcessConvertionFromJSONB(Schema* Current, std::unordered_map<size_t, std::string>* Map, const SchemaReadCallback& Callback);
			static bool ProcessConvertionToJSONB(Schema* Current, std::unordered_map<std::string, size_t>* Map, const SchemaWriteCallback& Callback);
			static bool GenerateNamingTable(const Schema* Current, std::unordered_map<std::string, size_t>* Map, size_t& Index);
		};

		class ED_OUT_TS Schedule final : public Reference<Schedule>
		{
		private:
			struct ThreadPtr
			{
				std::condition_variable Notify;
				std::mutex Update;
				std::thread Handle;
				std::thread::id Id;
				Difficulty Type = Difficulty::Count;
				bool Daemon = false;
			};

		public:
			enum class ThreadTask
			{
				Spawn,
				EnqueueTimer,
				EnqueueCoroutine,
				EnqueueTask,
				ConsumeCoroutine,
				ProcessTimer,
				ProcessCoroutine,
				ProcessTask,
				Awake,
				Sleep,
				Despawn
			};

			struct ED_OUT ThreadDebug
			{
				std::thread::id Id = std::thread::id();
				Difficulty Type = Difficulty::Count;
				ThreadTask State = ThreadTask::Spawn;
				size_t Tasks = 0;
			};

			typedef std::function<void(const ThreadDebug&)> ThreadDebugCallback;

		public:
			struct ED_OUT Desc
			{
				std::chrono::milliseconds Timeout = std::chrono::milliseconds(2000);
				size_t Threads[(size_t)Difficulty::Count] = { 1, 1, 1, 1 };
				size_t Memory = ED_STACK_SIZE;
				size_t Coroutines = 16;
				ActivityCallback Ping = nullptr;
				bool Parallel = true;

				Desc();
				void SetThreads(size_t Cores);
			};

		private:
			struct
			{
				std::vector<TaskCallback> Events;
				TaskCallback Tasks[ED_MAX_EVENTS];
				Costate* State = nullptr;
			} Dispatcher;

		private:
			ConcurrentQueuePtr* Queues[(size_t)Difficulty::Count];
			std::vector<ThreadPtr*> Threads[(size_t)Difficulty::Count];
			std::atomic<TaskId> Generation;
			std::mutex Exclusive;
			ThreadDebugCallback Debug;
			Desc Policy;
			bool Enqueue;
			bool Terminate;
			bool Active;
			bool Immediate;

		private:
			Schedule() noexcept;

		public:
			~Schedule() noexcept;
			TaskId SetInterval(uint64_t Milliseconds, const TaskCallback& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetInterval(uint64_t Milliseconds, TaskCallback&& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetSeqInterval(uint64_t Milliseconds, const SeqTaskCallback& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetSeqInterval(uint64_t Milliseconds, SeqTaskCallback&& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetTimeout(uint64_t Milliseconds, const TaskCallback& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetTimeout(uint64_t Milliseconds, TaskCallback&& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetSeqTimeout(uint64_t Milliseconds, const SeqTaskCallback& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetSeqTimeout(uint64_t Milliseconds, SeqTaskCallback&& Callback, Difficulty Type = Difficulty::Light);
			bool SetTask(const TaskCallback& Callback, Difficulty Type = Difficulty::Heavy);
			bool SetTask(TaskCallback&& Callback, Difficulty Type = Difficulty::Heavy);
			bool SetCoroutine(const TaskCallback& Callback);
			bool SetCoroutine(TaskCallback&& Callback);
			bool SetDebugCallback(const ThreadDebugCallback& Callback);
			bool SetImmediate(bool Enabled);
			bool ClearTimeout(TaskId WorkId);
			bool Start(const Desc& NewPolicy);
			bool Stop();
			bool Wakeup();
			bool Dispatch();
			bool IsActive() const;
			bool CanEnqueue() const;
			bool HasTasks(Difficulty Type) const;
			bool HasAnyTasks() const;
			size_t GetTotalThreads() const;
			size_t GetThreads(Difficulty Type) const;
			const Desc& GetPolicy() const;

		private:
			bool PostDebug(Difficulty Type, ThreadTask State, size_t Tasks);
			bool PostDebug(ThreadPtr* Ptr, ThreadTask State, size_t Tasks);
			bool ProcessTick(Difficulty Type);
			bool ProcessLoop(Difficulty Type, ThreadPtr* Thread);
			bool ThreadActive(ThreadPtr* Thread);
			bool ChunkCleanup();
			bool PushThread(Difficulty Type, bool IsDaemon);
			bool PopThread(ThreadPtr* Thread);
			std::chrono::microseconds GetTimeout(std::chrono::microseconds Clock);
			TaskId GetTaskId();

		public:
			static std::chrono::microseconds GetClock();
			static Schedule* Get();
			static void ExecutePromise(TaskCallback&& Callback);
			static bool Reset();
			static bool IsPresentAndActive();

		private:
			static Schedule* Singleton;
		};

		template <typename T>
		class ED_OUT Pool
		{
			static_assert(std::is_pointer<T>::value, "pool can be used only for pointer types");

		public:
			typedef T* Iterator;

		protected:
			size_t Count, Volume;
			T* Data;

		public:
			Pool() noexcept : Count(0), Volume(0), Data(nullptr)
			{
			}
			Pool(const Pool<T>& Ref) noexcept : Count(0), Volume(0), Data(nullptr)
			{
				if (Ref.Data != nullptr)
					Copy(Ref);
			}
			Pool(Pool<T>&& Ref) noexcept : Count(Ref.Count), Volume(Ref.Volume), Data(Ref.Data)
			{
				Ref.Count = 0;
				Ref.Volume = 0;
				Ref.Data = nullptr;
			}
			~Pool() noexcept
			{
				Release();
			}
			void Release()
			{
				ED_FREE(Data);
				Data = nullptr;
				Count = 0;
				Volume = 0;
			}
			void Reserve(size_t Capacity)
			{
				if (Capacity <= Volume)
					return;

				size_t Size = Capacity * sizeof(T);
				T* NewData = ED_MALLOC(T, Size);
				memset(NewData, 0, Size);
				
				if (Data != nullptr)
				{
					memcpy(NewData, Data, Count * sizeof(T));
					ED_FREE(Data);
				}

				Volume = Capacity;
				Data = NewData;
			}
			void Resize(size_t NewSize)
			{
				if (NewSize > Volume)
					Reserve(IncreaseCapacity(NewSize));

				Count = NewSize;
			}
			void Copy(const Pool<T>& Ref)
			{
				if (!Ref.Data)
					return;

				if (!Data || Ref.Count > Count)
				{
					ED_FREE(Data);
					Data = ED_MALLOC(T, Ref.Volume * sizeof(T));
					memset(Data, 0, Ref.Volume * sizeof(T));
				}

				memcpy(Data, Ref.Data, (size_t)(Ref.Count * sizeof(T)));
				Count = Ref.Count;
				Volume = Ref.Volume;
			}
			void Clear()
			{
				Count = 0;
			}
			Iterator Add(const T& Ref)
			{
				ED_ASSERT(Count < Volume, End(), "pool capacity overflow");
				Data[Count++] = Ref;
				return End() - 1;
			}
			Iterator AddUnbounded(const T& Ref)
			{
				if (Count + 1 >= Volume)
					Reserve(IncreaseCapacity(Count + 1));

				return Add(Ref);
			}
			Iterator AddIfNotExists(const T& Ref)
			{
				Iterator It = Find(Ref);
				if (It != End())
					return It;

				return Add(Ref);
			}
			Iterator RemoveAt(Iterator It)
			{
				ED_ASSERT(Count > 0, End(), "pool is empty");
				ED_ASSERT((size_t)(It - Data) >= 0 && (size_t)(It - Data) < Count, End(), "iterator ranges out of pool");

				Count--;
				Data[It - Data] = Data[Count];
				return It;
			}
			Iterator Remove(const T& Value)
			{
				Iterator It = Find(Value);
				if (It != End())
					RemoveAt(It);

				return It;
			}
			Iterator At(size_t Index) const
			{
				ED_ASSERT(Index < Count, End(), "index ranges out of pool");
				return Data + Index;
			}
			Iterator Find(const T& Ref)
			{
				auto End = Data + Count;
				for (auto It = Data; It != End; ++It)
				{
					if (*It == Ref)
						return It;
				}

				return End;
			}
			Iterator Begin() const
			{
				return Data;
			}
			Iterator End() const
			{
				return Data + Count;
			}
			Iterator begin() const
			{
				return Data;
			}
			Iterator end() const
			{
				return Data + Count;
			}
			size_t Size() const
			{
				return Count;
			}
			size_t Capacity() const
			{
				return Volume;
			}
			bool Empty() const
			{
				return !Count;
			}
			T* Get() const
			{
				return Data;
			}
			T& Front() const
			{
				return *Begin();
			}
			T& Back() const
			{
				return *(End() - 1);
			}
			T& operator [](size_t Index) const
			{
				return *(Data + Index);
			}
			T& operator [](size_t Index)
			{
				return *(Data + Index);
			}
			Pool<T>& operator =(const Pool<T>& Ref) noexcept
			{
				if (this == &Ref)
					return *this;

				Copy(Ref);
				return *this;
			}
			Pool<T>& operator =(Pool<T>&& Ref) noexcept
			{
				if (this == &Ref)
					return *this;

				Release();
				Count = Ref.Count;
				Volume = Ref.Volume;
				Data = Ref.Data;
				Ref.Count = 0;
				Ref.Volume = 0;
				Ref.Data = nullptr;
				return *this;
			}
			bool operator ==(const Pool<T>& Raw)
			{
				return Compare(Raw) == 0;
			}
			bool operator !=(const Pool<T>& Raw)
			{
				return Compare(Raw) != 0;
			}
			bool operator <(const Pool<T>& Raw)
			{
				return Compare(Raw) < 0;
			}
			bool operator <=(const Pool<T>& Raw)
			{
				return Compare(Raw) <= 0;
			}
			bool operator >(const Pool<T>& Raw)
			{
				return Compare(Raw) > 0;
			}
			bool operator >=(const Pool<T>& Raw)
			{
				return Compare(Raw) >= 0;
			}

		private:
			size_t IncreaseCapacity(size_t NewSize)
			{
				size_t Alpha = Volume > 4 ? (Volume + Volume / 2) : 8;
				return Alpha > NewSize ? Alpha : NewSize;
			}
		};

		template <typename T>
		class ED_OUT_TS Guarded
		{
		public:
			class Loaded
			{
			public:
				friend Guarded;

			private:
				Guarded* Base;

			public:
				Loaded(Loaded&& Other) noexcept : Base(Other.Base)
				{
					Other.Base = nullptr;
				}
				Loaded& operator =(Loaded&& Other) noexcept
				{
					if (&Other == this)
						return *this;

					Base = Other.Base;
					Other.Base = nullptr;
					return *this;
				}
				~Loaded() noexcept
				{
					Close();
				}
				void Close()
				{
					if (Base != nullptr)
					{
						Base->Mutex.LoadUnlock();
						Base = nullptr;
					}
				}
				operator bool() const
				{
					return Base != nullptr;
				}
				const T& operator*() const
				{
					ED_ASSERT(Base != nullptr, Base->Value, "value was not loaded");
					return Base->Value;
				}
				const T& Unwrap() const
				{
					ED_ASSERT(Base != nullptr, Base->Value, "value was not loaded");
					return Base->Value;
				}

			private:
				Loaded(Guarded* NewBase) noexcept : Base(NewBase)
				{
				}
			};

			class Stored
			{
			public:
				friend Guarded;

			private:
				Guarded* Base;

			public:
				Stored(Stored&& Other) noexcept : Base(Other.Base)
				{
					Other.Base = nullptr;
				}
				Stored& operator =(Stored&& Other) noexcept
				{
					if (&Other == this)
						return *this;

					Base = Other.Base;
					Other.Base = nullptr;
					return *this;
				}
				~Stored() noexcept
				{
					Close();
				}
				void Close()
				{
					if (Base != nullptr)
					{
						Base->Mutex.StoreUnlock();
						Base = nullptr;
					}
				}
				operator bool() const
				{
					return Base != nullptr;
				}
				T& operator*()
				{
					ED_ASSERT(Base != nullptr, Base->Value, "value was not stored");
					return Base->Value;
				}
				T& Unwrap()
				{
					ED_ASSERT(Base != nullptr, Base->Value, "value was not stored");
					return Base->Value;
				}

			private:
				Stored(Guarded* NewBase) noexcept : Base(NewBase)
				{
				}
			};

		public:
			friend Loaded;
			friend Stored;

		private:
			Guard Mutex;
			T Value;

		public:
			Guarded(const T& NewValue) noexcept : Value(NewValue)
			{
			}
			Guarded(const Guarded& Other) = delete;
			Guarded(Guarded&& Other) = delete;
			~Guarded() noexcept = default;
			Guarded& operator =(const Guarded& Other) = delete;
			Guarded& operator =(Guarded&& Other) = delete;
			Loaded TryLoad()
			{
				return Mutex.TryLoadLock() ? Loaded(this) : Loaded(nullptr);
			}
			Loaded Load()
			{
				Mutex.LoadLock();
				return Loaded(this);
			}
			Stored TryStore()
			{
				return Mutex.TryStoreLock() ? Stored(this) : Stored(nullptr);
			}
			Stored Store()
			{
				Mutex.StoreLock();
				return Stored(this);
			}
		};

		template <typename T>
		class ED_OUT_TS Awaitable
		{
		public:
			TaskCallback Event;
			std::mutex Update;
			std::atomic<uint32_t> Count;
			std::atomic<Deferred> Code;
			T Result;

		public:
			Awaitable() noexcept : Count(1), Code(Deferred::Pending), Result()
			{
			}
			Awaitable(const T& Value) noexcept : Count(1), Code(Deferred::Ready), Result(Value)
			{
			}
			Awaitable(T&& Value) noexcept : Count(1), Code(Deferred::Ready), Result(std::move(Value))
			{
			}
		};

		template <>
		class ED_OUT_TS Awaitable<void>
		{
		public:
			TaskCallback Event;
			std::mutex Update;
			std::atomic<uint32_t> Count;
			std::atomic<Deferred> Code;

		public:
			Awaitable() noexcept : Count(1), Code(Deferred::Pending)
			{
			}
		};

		template <typename T>
		class ED_OUT_TS Promise
		{
			static_assert(std::is_default_constructible<T>::value, "async cannot be used with non default constructible type");
			typedef Awaitable<T> Status;
			typedef T Type;

		private:
			template <typename F>
			struct Unwrap
			{
				typedef F type;
			};

			template <typename F>
			struct Unwrap<Promise<F>>
			{
				typedef F type;
			};

		private:
			Status* Data;

		public:
			Promise() noexcept : Data(ED_NEW(Status))
			{
			}
			explicit Promise(const T& Value) noexcept : Data(ED_NEW(Status, Value))
			{
			}
			explicit Promise(T&& Value) noexcept : Data(ED_NEW(Status, std::move(Value)))
			{
			}
			Promise(const Promise& Other) : Data(Other.Data)
			{
				AddRef();
			}
			Promise(Promise&& Other) noexcept : Data(Other.Data)
			{
				Other.Data = nullptr;
			}
			~Promise() noexcept
			{
				Release(Data);
			}
			Promise& operator= (const Promise& Other) = delete;
			Promise& operator= (Promise&& Other) noexcept
			{
				if (&Other == this)
					return *this;

				Release(Data);
				Data = Other.Data;
				Other.Data = nullptr;
				return *this;
			}
			void Set(const T& Other)
			{
				ED_ASSERT_V(Data != nullptr && Data->Code == Deferred::Pending, "async should be pending");
				std::unique_lock<std::mutex> Unique(Data->Update);
				Data->Code = Deferred::Ready;
				Data->Result = Other;
				Execute(Data);
			}
			void Set(T&& Other)
			{
				ED_ASSERT_V(Data != nullptr && Data->Code == Deferred::Pending, "async should be pending");
				std::unique_lock<std::mutex> Unique(Data->Update);
				Data->Code = Deferred::Ready;
				Data->Result = std::move(Other);
				Execute(Data);
			}
			void Set(const Promise& Other)
			{
				ED_ASSERT_V(Data != nullptr && Data->Code == Deferred::Pending, "async should be pending");
				Status* Copy = AddRef();
				Other.Await([Copy](T&& Value) mutable
				{
					std::unique_lock<std::mutex> Unique(Copy->Update);
					Copy->Code = Deferred::Ready;
					Copy->Result = std::move(Value);
					Execute(Copy);
					Release(Copy);
				});
			}
			void Await(std::function<void(T&&)>&& Callback) const noexcept
			{
				ED_ASSERT_V(Callback, "callback should be set");
				if (!IsPending())
					return Callback(std::move(Data->Result));

				Status* Copy = AddRef();
				Store([Copy, Callback = std::move(Callback)]() mutable
				{
					Callback(std::move(Copy->Result));
					Release(Copy);
				});
			}
			void Wait()
			{
				if (!IsPending())
					return;

				std::condition_variable Ready;
				std::mutex Waitable;
				Await([&](T&&)
				{
					std::unique_lock<std::mutex> Lock(Waitable);
					Ready.notify_all();
				});

				std::unique_lock<std::mutex> Lock(Waitable);
				Ready.wait(Lock, [&]()
				{
					return !IsPending();
				});
			}
			T&& Get() noexcept
			{
				Wait();
				return Load();
			}
			Deferred GetStatus() const noexcept
			{
				return Data ? Data->Code.load() : Deferred::None;
			}
			bool IsPending() const noexcept
			{
				return Data ? Data->Code.load() == Deferred::Pending : false;
			}
			template <typename R>
			Promise<R> Then(std::function<void(Promise<R>&, T&&)>&& Callback) const noexcept
			{
				ED_ASSERT(Data != nullptr && Callback, Promise<R>::Empty(), "async should be pending");

				Promise<R> Result; Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Callback(Result, std::move(Copy->Result));
					Release(Copy);
				});

				return Result;
			}
			template <typename R>
			Promise<typename Unwrap<R>::type> Then(std::function<R(T&&)>&& Callback) const noexcept
			{
				using F = typename Unwrap<R>::type;
				ED_ASSERT(Data != nullptr && Callback, Promise<F>::Empty(), "async should be pending");

				Promise<F> Result; Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Result.Set(std::move(Callback(std::move(Copy->Result))));
					Release(Copy);
				});

				return Result;
			}

		private:
			Promise(bool Unused1, bool Unused2) noexcept : Data(nullptr)
			{
			}
			Status* AddRef() const
			{
				if (Data != nullptr)
					++Data->Count;
				return Data;
			}
			T&& Load()
			{
				if (!Data)
					Data = ED_NEW(Status);

				return std::move(Data->Result);
			}
			void Store(TaskCallback&& Callback) const noexcept
			{
				std::unique_lock<std::mutex> Unique(Data->Update);
				Data->Event = std::move(Callback);
				if (Data->Code == Deferred::Ready)
					Execute(Data);
			}

		public:
			static Promise Empty() noexcept
			{
				return Promise(false, false);
			}

		private:
			static void Execute(Status* State) noexcept
			{
				if (State->Event)
					Schedule::ExecutePromise(std::move(State->Event));
			}
			static void Release(Status* State) noexcept
			{
				if (State != nullptr && !--State->Count)
					ED_DELETE(Status, State);
			}
		};
		
		template <>
		class ED_OUT_TS Promise<void>
		{
			typedef Awaitable<void> Status;
			typedef void Type;

		private:
			template <typename F>
			struct Unwrap
			{
				typedef F type;
			};

			template <typename F>
			struct Unwrap<Promise<F>>
			{
				typedef F type;
			};

		private:
			Status* Data;

		public:
			Promise() noexcept : Data(ED_NEW(Status))
			{
			}
			Promise(const Promise& Other) noexcept : Data(Other.Data)
			{
				AddRef();
			}
			Promise(Promise&& Other) noexcept : Data(Other.Data)
			{
				Other.Data = nullptr;
			}
			~Promise() noexcept
			{
				Release(Data);
			}
			Promise& operator= (const Promise& Other) = delete;
			Promise& operator= (Promise&& Other) noexcept
			{
				if (&Other == this)
					return *this;

				Release(Data);
				Data = Other.Data;
				Other.Data = nullptr;
				return *this;
			}
			void Set()
			{
				ED_ASSERT_V(Data != nullptr && Data->Code == Deferred::Pending, "async should be pending");
				std::unique_lock<std::mutex> Unique(Data->Update);
				Data->Code = Deferred::Ready;
				Execute(Data);
			}
			void Set(const Promise& Other)
			{
				ED_ASSERT_V(Data != nullptr && Data->Code == Deferred::Pending, "async should be pending");
				Status* Copy = AddRef();
				Other.Await([Copy]() mutable
				{
					std::unique_lock<std::mutex> Unique(Copy->Update);
					Copy->Code = Deferred::Ready;
					Execute(Copy);
					Release(Copy);
				});
			}
			void Await(std::function<void()>&& Callback) const noexcept
			{
				ED_ASSERT_V(Callback, "callback should be set");
				if (!IsPending())
					return Callback();

				Status* Copy = AddRef();
				Store([Copy, Callback = std::move(Callback)]() mutable
				{
					Callback();
					Release(Copy);
				});
			}
			void Wait()
			{
				if (!IsPending())
					return;

				std::condition_variable Ready;
				std::mutex Waitable;
				Await([&]()
				{
					std::unique_lock<std::mutex> Lock(Waitable);
					Ready.notify_all();
				});

				std::unique_lock<std::mutex> Lock(Waitable);
				Ready.wait(Lock, [&]()
				{
					return !IsPending();
				});
			}
			void Get() noexcept
			{
				Wait();
				Load();
			}
			Deferred GetStatus() const noexcept
			{
				return Data ? Data->Code.load() : Deferred::None;
			}
			bool IsPending() const noexcept
			{
				return Data ? Data->Code.load() == Deferred::Pending : false;
			}
			template <typename R>
			Promise<R> Then(std::function<void(Promise<R>&)>&& Callback) const noexcept
			{
				ED_ASSERT(Data != nullptr && Callback, Promise<R>::Empty(), "async should be pending");

				Promise<R> Result; Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Callback(Result);
					Release(Copy);
				});

				return Result;
			}
			template <typename R>
			Promise<typename Unwrap<R>::type> Then(std::function<R()>&& Callback) const noexcept
			{
				using F = typename Unwrap<R>::type;
				ED_ASSERT(Data != nullptr && Callback, Promise<F>::Empty(), "async should be pending");

				Promise<F> Result; Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Callback();
					Result.Set();
					Release(Copy);
				});

				return Result;
			}

		private:
			Promise(Status* Context) noexcept : Data(Context)
			{
				AddRef();
			}
			Status* AddRef() const
			{
				if (Data != nullptr)
					++Data->Count;
				return Data;
			}
			void Load() noexcept
			{
				if (!Data)
					Data = ED_NEW(Status);
			}
			void Store(TaskCallback&& Callback) const noexcept
			{
				std::unique_lock<std::mutex> Unique(Data->Update);
				Data->Event = std::move(Callback);
				if (Data->Code == Deferred::Ready)
					Execute(Data);
			}

		public:
			static Promise Empty() noexcept
			{
				return Promise((Status*)nullptr);
			}

		private:
			static void Execute(Status* State) noexcept
			{
				if (State->Event)
					Schedule::ExecutePromise(std::move(State->Event));
			}
			static void Release(Status* State) noexcept
			{
				if (State != nullptr && !--State->Count)
					ED_DELETE(Status, State);
			}
		};

		template <typename T>
		ED_OUT_TS inline T&& Coawait(Promise<T>&& Future, const char* Function = nullptr, const char* Expression = nullptr) noexcept
		{
			Costate* State; Coroutine* Base;
			if (!Costate::GetState(&State, &Base) || !Future.IsPending())
				return Future.Get();
#ifndef NDEBUG
			std::chrono::microseconds Time = Schedule::GetClock();
			if (Function != nullptr && Expression != nullptr)
				ED_WATCH_AT((void*)&Future, Function, Expression);
#endif
			State->Deactivate(Base, [&Future, &State, &Base]()
			{
				Future.Await([&State, &Base](T&&)
				{
					State->Activate(Base);
				});
			});
#ifndef NDEBUG
			if (Function != nullptr && Expression != nullptr)
			{
				int64_t Diff = (Schedule::GetClock() - Time).count();
				if (Diff > ED_TIMING_HANG * 1000)
					ED_WARN("[stall] async operation took %" PRIu64 " ms (%" PRIu64 " us)\t\nwhere: %s\n\texpression: %s\n\texpected: %" PRIu64 " ms at most", Diff / 1000, Diff, Function, Expression, (uint64_t)ED_TIMING_HANG);
				ED_UNWATCH((void*)&Future);
			}
#endif
			return Future.Get();
		}
		template <typename T>
		ED_OUT_TS inline Promise<T> Cotask(const std::function<T()>& Callback, Difficulty Type = Difficulty::Heavy) noexcept
		{
			ED_ASSERT(Callback, Promise<T>::Empty(), "callback should not be empty");

			Promise<T> Result;
			Schedule::Get()->SetTask([Result, Callback]() mutable
			{
				Result.Set(std::move(Callback()));
			}, Type);

			return Result;
		}
		template <typename T>
		ED_OUT_TS inline Promise<T> Cotask(std::function<T()>&& Callback, Difficulty Type = Difficulty::Heavy) noexcept
		{
			ED_ASSERT(Callback, Promise<T>::Empty(), "callback should not be empty");

			Promise<T> Result;
			Schedule::Get()->SetTask([Result, Callback = std::move(Callback)]() mutable
			{
				Result.Set(std::move(Callback()));
			}, Type);

			return Result;
		}
		template <typename T>
		ED_OUT_TS inline Promise<T> Coasync(const std::function<T()>& Callback, bool AlwaysEnqueue = false) noexcept
		{
			ED_ASSERT(Callback, Promise<T>::Empty(), "callback should not be empty");
			if (!AlwaysEnqueue && Costate::IsCoroutine())
				return Promise<T>(Callback());

			Promise<T> Result;
			Schedule::Get()->SetCoroutine([Result, Callback]() mutable
			{
				Result.Set(std::move(Callback()));
			});

			return Result;
		}
		template <typename T>
		ED_OUT_TS inline Promise<T> Coasync(std::function<T()>&& Callback, bool AlwaysEnqueue = false) noexcept
		{
			ED_ASSERT(Callback, Promise<T>::Empty(), "callback should not be empty");
			if (!AlwaysEnqueue && Costate::IsCoroutine())
				return Promise<T>(Callback());

			Promise<T> Result;
			Schedule::Get()->SetCoroutine([Result, Callback = std::move(Callback)]() mutable
			{
				Result.Set(std::move(Callback()));
			});

			return Result;
		}
		ED_OUT_TS inline Promise<bool> Cosleep(uint64_t Ms) noexcept
		{
			Promise<bool> Result;
			Schedule::Get()->SetTimeout(Ms, [Result]() mutable
			{
				Result.Set(true);
			}, Difficulty::Light);

			return Result;
		}
		ED_OUT_TS inline bool Coasync(const TaskCallback& Callback, bool AlwaysEnqueue = false) noexcept
		{
			ED_ASSERT(Callback, false, "callback should not be empty");
			if (!AlwaysEnqueue && Costate::IsCoroutine())
			{
				Callback();
				return true;
			}

			return Schedule::Get()->SetCoroutine(Callback);
		}
		ED_OUT_TS inline bool Coasync(TaskCallback&& Callback, bool AlwaysEnqueue = false) noexcept
		{
			ED_ASSERT(Callback, false, "callback should not be empty");
			if (!AlwaysEnqueue && Costate::IsCoroutine())
			{
				Callback();
				return true;
			}

			return Schedule::Get()->SetCoroutine(std::move(Callback));
		}
		ED_OUT_TS inline bool Cosuspend() noexcept
		{
			Costate* State = Costate::Get();
			ED_ASSERT(State != nullptr, false, "cannot call suspend outside coroutine");

			return State->Suspend();
		}
		ED_OUT_TS inline String Form(const char* Format, ...) noexcept
		{
			ED_ASSERT(Format != nullptr, String(), "format should be set");

			va_list Args;
			va_start(Args, Format);

			char Buffer[ED_BIG_CHUNK_SIZE];
			int Size = vsnprintf(Buffer, sizeof(Buffer), Format, Args);
			va_end(Args);

			return String(Buffer, Size > ED_BIG_CHUNK_SIZE ? ED_BIG_CHUNK_SIZE : (size_t)Size);
		}
		template <size_t Size>
		ED_OUT_TS constexpr uint64_t Shuffle(const char Source[Size]) noexcept
		{
			uint64_t Result = 0xcbf29ce484222325;
			for (size_t i = 0; i < Size; i++)
			{
				Result ^= Source[i];
				Result *= 1099511628211;
			}

			return Result;
		}
	}
}
#endif
