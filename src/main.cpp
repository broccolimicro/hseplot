/*
 * main.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: nbingham
 */

#include <common/standard.h>
#include <parse/parse.h>
#include <parse/default/block_comment.h>
#include <parse/default/line_comment.h>
#include <parse_chp/composition.h>
#include <hse/graph.h>
#include <hse/simulator.h>
#include <hse/elaborator.h>
#include <interpret_hse/import.h>
#include <interpret_hse/export.h>
#include <interpret_boolean/export.h>
#include <interpret_boolean/import.h>
#include <ucs/variable.h>

#ifdef GRAPHVIZ_SUPPORTED
namespace graphviz
{
	#include <graphviz/cgraph.h>
	#include <graphviz/gvc.h>
}
#endif

void print_help()
{
	cout << "Usage: plot [options] file..." << endl;
	cout << "Options:" << endl;
	cout << " -h,--help      Display this information" << endl;
	cout << "    --version   Display version information" << endl;
	cout << " -v,--verbose   Display verbose messages" << endl;
	cout << " -d,--debug     Display internal debugging messages" << endl;
	cout << endl;
	cout << " -o             Specify the output file name" << endl;
	cout << "    formats other than 'dot' are passed onto graphviz dot for rendering" << endl;
	cout << " -s,--sync\t\tRender half synchronization actions" << endl;
}

void print_version()
{
	cout << "plot 1.0.0" << endl;
	cout << "Copyright (C) 2013 Sol Union." << endl;
	cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	cout << endl;
}

int main(int argc, char **argv)
{
	configuration config;
	config.set_working_directory(argv[0]);
	tokenizer hse_tokens;
	tokenizer astg_tokens;
	parse_chp::composition::register_syntax(hse_tokens);
	parse_astg::graph::register_syntax(astg_tokens);
	hse_tokens.register_token<parse::block_comment>(false);
	hse_tokens.register_token<parse::line_comment>(false);
	string ofilename = "a.png";
	string oformat = "png";

	bool labels = false;

	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];
		if (arg == "--help" || arg == "-h")			// Help
		{
			print_help();
			return 0;
		}
		else if (arg == "--version")	// Version Information
		{
			print_version();
			return 0;
		}
		else if (arg == "--verbose" || arg == "-v")
			set_verbose(true);
		else if (arg == "--debug" || arg == "-d")
			set_debug(true);
		else if (arg == "--labels" || arg == "-l")
			labels = true;
		else if (arg == "-o")
		{
			i++;
			if (i < argc)
			{
				ofilename = argv[i];

				size_t loc = ofilename.find_last_of(".");
				if (loc != ofilename.npos)
					oformat = ofilename.substr(loc+1);
			}
			else
			{
				error("", "expected output filename", __FILE__, __LINE__);
				return 1;
			}
		}
		else
		{
			string filename = argv[i];
			int dot = filename.find_last_of(".");
			string format = "";
			if (dot != string::npos)
				format = filename.substr(dot+1);
			if (format == "hse")
				config.load(hse_tokens, filename, "");
			else if (format == "astg")
				config.load(astg_tokens, filename, "");
			else
				printf("unrecognized file format '%s'\n", format.c_str());
		}
	}

	if (is_clean() && hse_tokens.segments.size() + astg_tokens.segments.size() > 0)
	{
		hse::graph g;
		ucs::variable_set v;

		hse_tokens.increment(false);
		hse_tokens.expect<parse_chp::composition>();
		while (hse_tokens.decrement(__FILE__, __LINE__))
		{
			parse_chp::composition syntax(hse_tokens);
			g.merge(hse::parallel, import_graph(syntax, v, 0, &hse_tokens, true));

			hse_tokens.increment(false);
			hse_tokens.expect<parse_chp::composition>();
		}

		astg_tokens.increment(false);
		astg_tokens.expect<parse_astg::graph>();
		while (astg_tokens.decrement(__FILE__, __LINE__))
		{
			parse_astg::graph syntax(astg_tokens);
			g.merge(hse::parallel, import_graph(syntax, v, &astg_tokens));

			astg_tokens.increment(false);
			astg_tokens.expect<parse_astg::graph>();
		}
		g.post_process(v, true);
		g.check_variables(v);

		if (is_clean())
		{
			string dot = export_graph(g, v, labels).to_string();

			if (oformat == "dot")
			{
				FILE *file = fopen(ofilename.c_str(), "w");
				fprintf(file, "%s\n", dot.c_str());
				fclose(file);
			}
			else
			{
#ifdef GRAPHVIZ_SUPPORTED
				graphviz::Agraph_t* G = graphviz::agmemread(dot.c_str());
				graphviz::GVC_t* gvc = graphviz::gvContext();
				graphviz::gvLayout(gvc, G, "dot");
				graphviz::gvRenderFilename(gvc, G, oformat.c_str(), ofilename.c_str());
				graphviz::gvFreeLayout(gvc, G);
				graphviz::agclose(G);
				graphviz::gvFreeContext(gvc);
#else
				
				string tfilename = ofilename.substr(0, ofilename.find_last_of("."));
				FILE *temp = NULL;
				int num = 0;
				for (; temp == NULL; num++)
					temp = fopen((tfilename + (num > 0 ? to_string(num) : "") + ".dot").c_str(), "wx");
				num--;
				tfilename += (num > 0 ? to_string(num) : "") + ".dot";

				fprintf(temp, "%s\n", dot.c_str());
				fclose(temp);

				if (system(("dot -T" + oformat + " " + tfilename + " > " + ofilename).c_str()) != 0)
					error("", "Graphviz DOT not supported", __FILE__, __LINE__);
				else if (system(("rm -f " + tfilename).c_str()) != 0)
					warning("", "Temporary files not cleaned up", __FILE__, __LINE__);
#endif
			}
		}
	}

	complete();
	return is_clean();
}
