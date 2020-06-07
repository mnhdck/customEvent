#include <vector>
#include <iterator>

#ifdef WIN32
#define PTO_ULONG(v) ((unsigned long)(unsigned __int64)v)
#else
#define PTO_ULONG(v) ((unsigned long)v)
#endif

class EventIDFactory
{
public:
	unsigned long GetEventSenderNewID(){ m_vecIDList.push_back(++m_lMaxID); return m_lMaxID; }
	bool DelEventSenderID(long nID){
		std::vector<long>::iterator it = FindID(nID);
		if (it != m_vecIDList.end()){
			m_vecIDList.erase(it);
			return true;
		}
		else
			return false;
	}
	bool IsExistEventSenderID(long nID){ return (m_vecIDList.end() == FindID(nID)); }
private:
	std::vector<long>::iterator FindID(int nID){
		std::vector<long>::iterator it;
		for (it = m_vecIDList.begin(); it != m_vecIDList.end();){
			if (*it == nID) return it;
			else it++;
		}
		return m_vecIDList.end();
	}
	std::vector<long> m_vecIDList;
	unsigned long m_lMaxID;
};

inline EventIDFactory& GetEventFactory()
{
	static EventIDFactory s_eventIDFactory;
	return s_eventIDFactory;
}

//Event Define
#define Event_Def(eventName, ...) \
public: \
	template<typename... ARGS> \
	class E_##eventName{ \
	public: \
		E_##eventName():m_bIsSending(false), m_nSenderID(-1){m_nSenderID = GetEventFactory().GetEventSenderNewID();} \
		~E_##eventName(){GetEventFactory().DelEventSenderID(m_nSenderID);} \
		typedef void (EventAccepter::*EventType)(__VA_ARGS__); \
		EventType eType; \
		struct Info{ \
			EventType fun; \
			EventAccepter* accepter; \
			int offset; \
			Info():fun(NULL), accepter(NULL), offset(0){} \
				}; \
			void Add(EventAccepter* accepter, EventType func, int pointerOffset) \
				{ \
		for (Info info : m_vecList){ if (info.accepter == accepter && info.fun == func) return; } \
			Info info; info.accepter = accepter; info.fun = func; info.offset = pointerOffset; \
			if(m_bIsSending) m_vecAdd.push_back(info); \
						else m_vecList.push_back(info); \
				} \
		void Send(ARGS... args){ \
			m_bIsSending = true; \
			int nSenderId = m_nSenderID; \
			for (Info& info : m_vecList){ \
				((info.accepter)->*info.fun)(args...); \
				if(!GetEventFactory().IsExistEventSenderID(nSenderId)) return ;} \
			m_bIsSending = false; \
			std::copy(m_vecAdd.begin(), m_vecAdd.end(), std::back_inserter(m_vecList)); m_vecAdd.clear(); \
			for (Info& info : m_vecRemove) { Remove(info.accepter, info.fun);} m_vecRemove.clear(); } \
		void Remove(EventAccepter* accepter, EventType func) \
				{ \
			if(m_bIsSending) { \
				Info info; info.accepter = accepter; info.fun = func; \
				m_vecRemove.push_back(info);} \
						else {for (auto iter = m_vecList.begin(); iter != m_vecList.end(); ){if(iter->accepter ==accepter && iter->fun == func) {iter= m_vecList.erase(iter);}else iter++;}} \
				} \
	private: \
		std::vector<Info> m_vecList; \
		std::vector<Info> m_vecAdd; \
		std::vector<Info> m_vecRemove; \
		bool m_bIsSending; \
		int m_nSenderID; \
		}; \
	E_##eventName<__VA_ARGS__> eventName;

#define ERegister(sender, eventName, accepter, slot) (sender)->eventName.Add(static_cast<EventAccepter*>((void*)accepter), static_cast<decltype((sender)->eventName.eType)>(slot), PTO_ULONG(accepter) - PTO_ULONG(static_cast<EventAccepter*>(accepter)))
#define EUnRegister(sender, eventName, accepter, slot) if(sender){(sender)->eventName.Remove(static_cast<EventAccepter*>((void*)accepter), static_cast<decltype((sender)->eventName.eType)>(slot));}
#define ESendEvent(sender, eventName, ...) (sender)->eventName.Send(__VA_ARGS__)

class EventAccepter
{
};
