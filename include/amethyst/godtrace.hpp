#pragma once

#include <amethyst/id.hpp>
#include <amethyst/types.hpp>

namespace Amy {
class GTrace {
 public:
  GTrace() = default;
  ~GTrace() = default;

  class Stats {
   public:
    /**
     * Constructor taking a lengh for the List
     * @param l Lengh of the data list
     */
    Stats(int l) : len(l), val(l, 0) {}
    ~Stats() = default;

    AMY_SHARED(Stats);

    /**
     * Add a New Value to the list
     * @param v value to add
     */
    void Add(ull v) {
      val[idx] = v;
      idx = next(idx);
      num_val = std::min(num_val + 1, len);
    }

    /**
     * Get Avarage Num
     * @return Average
     */
    ull GetAverage() {
      if (!num_val) return 0.f;
      ull res = 0;
      for (int i = 0; i < num_val; i++) {
        res += val[smart_idx(i)];
      }
      return res / num_val;
    }

    /**
     * Get Minimum Num
     * @return Minimum value
     */
    ull GetMin() {
      if (!num_val) return 0.f;
      ull res = std::numeric_limits<ull>::max();
      for (int i = 0; i < num_val; i++) {
        res = std::min(val[smart_idx(i)], res);
      }
      return res;
    }

    /**
     * Get Maximum Value
     * @return Max Value
     */
    ull GetMax() {
      if (!num_val) return 0.f;
      ull res = 0;
      for (int i = 0; i < num_val; i++) {
        res = std::max(val[smart_idx(i)], res);
      }
      return res;
    }

    /**
     * Clear the List
     */
    void Clear() {
      val.assign(len, 0);
      idx = 0;
      num_val = 0;
    }

    /**
     * Get Data Buffer
     * @return data bufer (not edidable)
     */
    const std::vector<ull>& GetData() { return val; }
    /**
     * Access an element in the list [not edidable]
     * @return value to access
     */
    const ull& operator[](int i) { return val[smart_idx(i)]; }
    /**
     * Get List Lengh
     * @return Lengh
     */
    const size_t GetLen() { return len; }
    /**
     * Get Number of Values
     * @return number of values
     */
    const size_t GetNumValues() { return num_val; }

   private:
    /**
     * Get the Next Position to write to
     * @param c current position
     * @return next position
     */
    size_t next(size_t c) const { return (c + 1) % len; }
    /**
     * Smart Indexing in for loops to make sure to
     * not index a value that was not set yet
     * @param v pos in for loop
     * @return indexing pos
     */
    size_t smart_idx(size_t v) const { return (idx + len - num_val + v) % len; }

    /** Lengh of the list */
    int len = 0;
    /** Value Storage */
    std::vector<ull> val;
    int idx = 0;
    int num_val = 0;
  };

  class Trace {
   public:
    Trace() { protocol = Stats::New(60); }
    ~Trace() = default;

    AMY_SHARED(Trace);

    void SetID(ksr v) { Name = v; }
    kstr GetID() { return Name; }
    void SetStart(ull v) { Start = v; }
    ull GetStart() { return Start; }
    void SetEnd(ull v) {
      End = v;
      Diff = End - Start;
      protocol->Add(GetLastDiff());
    }
    ull GetEnd() { return End; }
    ull GetLastDiff() { return Diff; }
    Stats::Ref GetProtocol() { return protocol; }

   private:
    str Name;
    ull Start = 0;
    ull End = 0;
    ull Diff = 0;
    Stats::Ref protocol;
  };

  /**
   * Begin a Trace
   * @param id Name of the Trace
   */
  static void Beg(ksr id);
  /**
   * End a Trace
   * @param id Name of the Trace
   */
  static void End(ksr id);

  /**
   * Collect Start end end of the trace by tracking
   * when the Scope object goes out of scope
   *
   * Example:
   * ```cpp
   * void SomeFunction() {
   *    // Create a Scoped Trace called "SomeFunc"
   *    PD::TT::Scope st("SomeFunc");
   *    // Do your functions stuff
   *    // End at the end it goes out of
   *    // scope which collects the end time
   * }
   * ```
   */
  class Scope {
   public:
    /**
     * Constructor requiring a Name for the Trace
     * @param id Name of the Trace
     */
    Scope(ksr id) {
      this->ID = id;
      GTrace::Beg(id);
    }
    /**
     * Deconstructor getting the end time when going out of scope
     */
    ~Scope() { GTrace::End(ID); }

   private:
    /** Trace Name/ID */
    str ID;
  };
  using TraceMap = std::unordered_map<std::string, Trace::Ref>;
  static TraceMap& GetTraceMap() { return GetTraceMap(); }
  static Trace::Ref& GetTraceRef(ksr id) {
    if (!pTraces.count(id)) {
      pTraces[id] = Trace::New();
      pTraces[id]->SetID(id);
    }
    return pTraces[id];
  }
  static bool TraceExist(ksr id) { return pTraces.count(id); }

 private:
  static TraceMap pTraces;
};
}  // namespace Amy