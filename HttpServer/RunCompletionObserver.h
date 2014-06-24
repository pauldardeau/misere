// Copyright Paul Dardeau, SwampBits LLC 2014
// BSD License

#ifndef HttpServer_RunCompletionObserver_h
#define HttpServer_RunCompletionObserver_h


class Runnable;

/*!
 * RunCompletionObserver is an interface used for callbacks to be notified
 * when a Runnable has completed its run.
 */
class RunCompletionObserver
{
public:
   virtual ~RunCompletionObserver() noexcept {}
   virtual void notifyRunComplete(Runnable* pRunnable) noexcept = 0;
};


#endif
