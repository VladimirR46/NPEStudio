%
% Copyright 2016 MathWorks, Inc
%
function flightsByCarrierDemo(master, inputFile)
import matlab.compiler.mlspark.*
%%
% Set up datastore for airlinesmall.csv
%
ds = datastore( ...
    inputFile, ...
    'datastoretype','tabulartext', ...
    'TreatAsMissing','NA', ...
    'SelectedVariableNames','UniqueCarrier');%%
%% Set up spark to use yarn resource manager....
%   1 thread per executor task
%   1 executor task
sparkProperties = containers.Map( ...
    {'spark.executor.cores','spark.executor.instances'}, ...
    {'1','1'});
conf = SparkConf( ...
    'AppName','flightsByCarrierDemo', ...
    'Master',master, ...
    'SparkProperties',sparkProperties );
sc = SparkContext(conf);
%%
% Create an rdd that contains the number of flights per airline.
countrdd = sc.datastoreToRDD(ds).flatMap(@mapcarriertocountusingds) ...
     .reduceByKey( @(acc,value) acc+value) ...
     .map( @(keyvalue) sprintf('%s,%i',keyvalue{1},keyvalue{2}));
countrdd.saveAsTextFile('flightsByCarrierResults');
