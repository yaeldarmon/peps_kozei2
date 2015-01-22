using PricerWrapper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using WebSite.Models;

namespace WebSite.Controllers
{
    public class HomeController : Controller
    {
        private KozeiDBEntities _db = new KozeiDBEntities();
        private FunctionsPricer func = new FunctionsPricer();

        public ActionResult Index()
        {
            return View("Index");
        }

        public ActionResult Test()
        {
            // Some test
            return View("Index");
        }

        public ActionResult ComputePriceAt0()
        {
            DateTime date = new DateTime(2014, 7, 11);
            string format = "dd/MM/yyyy";
            string s = date.ToString(format);

            // Call model to calculate
            double prix = 0, ic = 0;
            func.getPrice(out prix, out ic);

            return Json(new { date = s, price = prix, ic = ic }, JsonRequestBehavior.AllowGet);
        }

        public ActionResult ComputePriceNow()
        {
            DateTime start = new DateTime(2014, 7, 11);
            DateTime date = DateTime.Now;
            string format = "dd/MM/yyyy HH:mm:ss";
            string s = date.ToString(format);

            // Call model to calculate
            double prix = 0, ic = 0;
            double t = (date - start).TotalDays / 365.0;
            func.getPrice(out prix, out ic, t);

            return Json(new { date = s, price = prix, ic = ic }, JsonRequestBehavior.AllowGet);
        }

        public ActionResult GetHistoricalPrice()
        {
            List<Value> list = _db.Value.Where(v => v.price_portfolio_id == 1).ToList();

            var entries = list.Select(v => new { date = v.date, price = v.value1 });
            return Json(entries, JsonRequestBehavior.AllowGet);
        }


        #region Hedge

        /**
         * Calcul la couverture et renvoie le résultat
         */
        public JsonResult ComputeHedge()
        {
            // Hedge ; 32 rebalancements
            func.hedge(32);
            return GetLastHedge();
        }

        /** 
         * Récupère le tableau des deltas stockés dans la base de données
         * 
         */

		 
	    /// <summary>
        /// Récupère le tableau des deltas stockés dans la base de données
        /// Renvoie : Dates + Assets + Values
        /// </summary>
        public JsonResult GetLastHedge()
        {
            // Récupération de la composition du portefeuille
            List<Portfolio_Part> list = _db.Portfolio_Part.Where(v => v.portfolio_id == 1 && v.date != null
                                                                 && v.part != null && v.financial_product_id != null)
                                                          .ToList();


            // Récupération de la liste des dates
            List<double> listDate = list.Select(p => (double) p.date)
                                        .Distinct()
                                        .OrderBy(d => d)
                                        .ToList();

            // Récupération de la liste des actifs (id, nom)
            Dictionary<int, string> dictAsset = _db.Share.Select(s => new { s.Id, s.ticker })
                                                         .OrderBy(s => s.ticker)
                                                         .ToDictionary(s => s.Id, s => s.ticker);

            // Récupération de l'ordre des actifs (idAsset, indice)
            List<int> listAssetId = _db.Share.OrderBy(s => s.ticker)
                                             .Select(s => s.Id)
                                             .ToList();
            
            Dictionary<int, int> dictAssetId = new Dictionary<int, int>();
            for (int i = 0; i < listAssetId.Count; i++)
                dictAssetId.Add(listAssetId[i], i);


            // Initialisation du tableau des valeurs de delta
            double[] TAB = new double[listDate.Count * dictAsset.Count];
            for (int i = 0; i < listDate.Count * dictAsset.Count; i++)
                TAB[i] = 0.0;

            // On ajoute chaque delta au tableau
            foreach (Portfolio_Part pp in list)
            {
                int i = dictAssetId[(int)pp.financial_product_id];
                int j = listDate.IndexOf((double) pp.date);
                TAB[i * listDate.Count + j] = (double)pp.part;
            }
            
            List<double> listValue = TAB.ToList();

            // Date + Asset + Value
            string format = "dd/MM/yyyy";
            DateTime start = new DateTime(2014, 7, 11);
            var entriesDate = listDate.Select(d => new { date = start.AddDays(d*365).ToString(format) });

            var entriesAsset = dictAsset.Select(k => new { name = k.Value });
            var entriesValue = listValue.Select(v => new { value = Math.Round(v, 2) });

            var entries = new { date = entriesDate, asset = entriesAsset, value = entriesValue };
            return Json(entries, JsonRequestBehavior.AllowGet);
        }

        #endregion Hedge
    }
}
