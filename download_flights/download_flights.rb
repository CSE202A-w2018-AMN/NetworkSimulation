#!/usr/bin/env ruby

=begin

Downloads KML files for flights from FlightAware

Usage: download_flights.rb airport-code count

airport-code: 4-letter ICAO code of the airport to find flights to
count:  Number of arriving flights to download
destination: Path to the folder where flights should be written

=end

require 'mechanize'

def main
    args = Args.new()

    scraper = Mechanize.new
    scraper.pluggable_parser.default = Mechanize::Download
    scraper.history_added = Proc.new { sleep 0.5 }

    base_url = 'https://flightaware.com'
    offset = 0;
    download_count = 0
    while download_count < args.count
        arrivals_url = make_arrivals_url(args.airport, offset)
        puts "Reading flight list from #{arrivals_url}"
        scraper.get(arrivals_url) do |arrivals|
            flight_rows = arrivals.search('table.prettyTable.fullWidth tr')
            # Skip the two heade rows
            flight_rows = flight_rows[2..-1]

            flight_rows.each do |row|
                flight_link = row.search('a')[0]
                flight_url = base_url + flight_link.attributes['href'].value + '/google_earth'
                puts "Downloading KML #{flight_url}"
                scraper.get(flight_url).save()
                download_count += 1
                if download_count == args.count
                    break
                end
            end
            # Offset for next page
            offset += 20
        end
    end
    puts "Downloaded #{download_count} flights"
end

def make_arrivals_url(airport_code, offset)
    "https://flightaware.com/live/airport/#{airport_code}/arrivals?;offset=#{offset};order=actualarrivaltime;sort=DESC"
end

class Args
    attr_reader :airport, :count
    def initialize
        if ARGV.length != 2
            puts 'Usage: download_flights.rb airport-code count destination'
            exit -1
        end
        @airport = ARGV[0].upcase
        @count = ARGV[1].to_i
    end
end

main
