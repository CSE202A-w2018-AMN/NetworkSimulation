#!/usr/bin/env ruby

=begin

Finds flights in a folder that are transatlantic and copies them to
another foler

Usage: filter_flights.rb source-directory transatlantic-directory

=end

require 'nokogiri'
require 'fileutils'

def main
    source = ARGV[0]
    dest = ARGV[1]

    if !source || !dest
        puts 'Usage: filter_flights.rb source-directory transatlantic-directory'
        exit -1
    end

    puts "#{source} => #{dest}"
    Dir.foreach(source) do |entry_name|
        path = source + '/' + entry_name
        if File.file? path
            flight = Flight.new(path)
            if flight.transatlantic
                puts "Transatlantic from #{flight.origin}"
                FileUtils.cp(path, dest + '/' + entry_name)
            end
        end
    end
end

module Continent
    NORTH_AMERICA = 0
    EUROPE = 1
    OTHER = 2

    def Continent.from_code(code)
        if code.start_with?('C') \
            || code.start_with?('K') \
            || code.start_with?('m') \
            || code.start_with?('T')
            NORTH_AMERICA
        elsif code.start_with?('E') \
            || code.start_with?('L') \
            || code.start_with?('B')
            EUROPE
        else
            OTHER
        end
    end
end

class Flight
    attr_reader :path, :origin, :destination
    def initialize(path)
        @path = path
        # Open file, parse XML, find /Document/name
        xml = Nokogiri::XML(File.open(@path))
        # puts xml
        doc_name = xml.css('kml > Document > name')[0].text()

        # Expect: Three airplane symbols, then two airport blocks separated
        # by a slash
        name_rx = /^[^✈]+✈[^✈]+✈[^✈]+✈\s*(?<origin>.+?)\s*-\s*(?<destination>.+?)\s*$/

        match = name_rx.match doc_name
        if match
            @origin = trim_airport_code(match[:origin])
            @destination = trim_airport_code(match[:destination])
        else
            raise 'Failed to parse flight description ' + doc_name
        end
    end

    def transatlantic
        origin_continent = Continent.from_code(@origin)
        destination_continent = Continent.from_code(@destination)
        return origin_continent == Continent::EUROPE && destination_continent == Continent::NORTH_AMERICA \
            || origin_continent == Continent::NORTH_AMERICA && destination_continent == Continent::EUROPE
    end
end

def trim_airport_code(code)
    airport_code_rx = /[A-Z]{4}/
    match = airport_code_rx.match code
    if match
        match[0]
    else
        raise 'Failed to parse airport code ' + code
    end
end

main
