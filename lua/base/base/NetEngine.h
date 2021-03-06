#ifndef NetEngine_h__
#define NetEngine_h__

#include <vector>
#include <thread>
#include <mutex>

class NetSocket;
class NetEngine
{
public:
	static NetEngine *instance();
	virtual ~NetEngine();

	void start();
	void stop();
	void run();
	inline bool isStarted() { return m_thread ? true : false; }

	void add(NetSocket *sock);
	void remove(NetSocket *sock);
	bool find(NetSocket *sock, size_t &index);

	void update();

private:
	NetEngine();
	static void thread_task(NetEngine *engine);

private:
	std::thread *m_thread;
	bool m_bRunning;
	std::vector<NetSocket *> m_socks;
	std::mutex m_socks_mutex;
};

extern NetSocket *GetNetImp();

#endif // NetEngine_h__
