#include <DysectAPI.h>

DysectStatus DysectAPI::onProcStart(int argc, char **argv) {

    // test some basic functionality for all processes
    Probe *codeBreakpoint = new Probe(Code::location("dysect_test.cpp#81"),
                                Domain::world(),
                                Acts(Act::stackTrace(),
                                     Act::trace("The ranks are: @ranks()"),
                                     Act::trace("The times are: @time()"),
                                     Act::trace("Function is '@function()' and Location is '@location()'"),
                                     Act::stat()));

    // examine some variables for a few processes
    // rank 1 should be delayed, hence we also test the probe timeout
    Probe *vars = new Probe(Code::location("dysect_test.cpp#102"),
                            Domain::group("1,4,5", 2000),
                            Acts(Act::stackTrace(),
                                 Act::trace("vars Location is @function():@location() and rank is '@desc(rank)'"),
                                 Act::trace("frank = rank * 1.1 is '@desc(frank)' and drank = rank * 2.2 is '@desc(drank)'"),
                                 Act::trace("ip is '@desc(ip)'"),
                                 Act::trace("The ranks are: @ranks() and The times are: @time()"),
                                 Act::trace("First rank is @first(rank), last rank is @last(rank)"),
                                 Act::trace("Min lrank is '@min(lrank)', Max drank is '@max(drank)'"),
                                 Act::stat()),
                                 stay);

    // test a linked probe
    Probe *linked = new Probe(Time::within(0),
                              Domain::group("1,4,5", 1000),
                              Acts(Act::trace("Linked probe is at @location()'"),
                                   Act::trace("The ranks are: @ranks()"),
                                   Act::trace("The times are: @time()"),
                                   Act::stackTrace(),
                                   Act::detach()));

    vars->link(linked);

    // test a persistent probe with "stay"
    // this should be rank 1 in its sleep loop
    Probe *inLoop = new Probe(Code::location("dysect_test.cpp#94"),
                              Domain::world(1001, true),
                              Act::trace("in Loop Location is '@location(), rank = @desc(rank), i = '@desc(i)"),
                              stay);

    // tasks 4 and 5 have detached and should not be in STAT trace
    Probe *afterBarrier = new Probe(Code::location("dysect_test.cpp#60"),
                                    Domain::world(5000, true),
                                    Acts(Act::trace("After Barrier Location is '@location()'"),
                                         Act::stat()));


    ProbeTree::addRoot(codeBreakpoint);
    ProbeTree::addRoot(vars);
    ProbeTree::addRoot(afterBarrier);
    ProbeTree::addRoot(inLoop);

    return DysectOK;
}

