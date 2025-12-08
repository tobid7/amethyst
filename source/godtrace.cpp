#include <amethyst/godtrace.hpp>
#include <amethyst/utils.hpp>

Amy::GTrace::TraceMap Amy::GTrace::pTraces;

void Amy::GTrace::Beg(ksr id) {
  auto trace = GetTraceRef(id);
  trace->SetStart(Amy::Utils::GetTimeNano());
}

void Amy::GTrace::End(ksr id) {
  auto trace = GetTraceRef(id);
  trace->SetEnd(Amy::Utils::GetTimeNano());
}