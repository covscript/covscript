#!/usr/bin/env cs
import streams

function echo(o)
    if o == null
        system.out.println("null")
    else
        system.out.println(o)
    end
end

@begin
echo(
    streams.iterate(1, [](x) -> x * 2)
        .map([](x) -> x - 1)
        .filter([](x) -> x >= 100000)
        .skip(5)
        .take_while([](x) -> x <= 5000000)
        .for_each(system.out.println)
)
@end

